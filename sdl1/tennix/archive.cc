
/**
 *
 * Tennix Archive File Format
 * Copyright (C) 2009-2010 Thomas Perl <thp@thpinfo.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA  02110-1301, USA.
 *
 **/

#include <iostream>

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <arpa/inet.h>
#include <WinSock2.h>

#include "archive.hh"

TennixArchive::TennixArchive(const char* filename, const char* fallback)
{
    fp = fopen(filename, "rb");
    if (fp == NULL && fallback != NULL) {
        fp = fopen(fallback, "rb");
    }
    assert(fp != NULL);

    offset = sizeof(TennixArchiveHeader)*fread(&(header), sizeof(TennixArchiveHeader), 1, fp);
    assert(offset == sizeof(TennixArchiveHeader));
    assert(strncmp(header.header, TENNIX_ARCHIVE_HEADER, TENNIX_ARCHIVE_HEADER_LEN) == 0);
    assert(header.versionmajor == TENNIX_ARCHIVE_VERSIONMAJOR);
    assert(header.versionminor == TENNIX_ARCHIVE_VERSIONMINOR);

    items = (TennixArchiveItem*)calloc(header.items, sizeof(TennixArchiveItem));
    assert(items != NULL);
    offset += sizeof(TennixArchiveItem)*fread(items, sizeof(TennixArchiveItem), header.items, fp);
    assert(offset == sizeof(TennixArchiveHeader) + header.items*sizeof(TennixArchiveItem));

    xormem((char*)(items), header.items*sizeof(TennixArchiveItem), header.key);

    for (int i=0; i<header.items; i++) {
        /* convert offset + length from network byte order */
        items[i].offset = ntohl(items[i].offset);
        items[i].length = ntohl(items[i].length);
    }

    current_item = 0;

    building = 0;
}

std::ostream&
operator<<(std::ostream& out, TennixArchiveHeader& header)
{
    out << "Header: " << header.header << std::endl;
    out << "Version: " <<
        (int)header.versionmajor << '.' <<
        (int)header.versionminor << std::endl;
    out << "Master key: " << header.key << std::endl;
    out << "Items: " << header.items;

    return out;
}

std::ostream&
operator<<(std::ostream& out, TennixArchiveItem& item)
{
    out << "File: " << item.filename << std::endl;
    out << "Size: " << item.length << std::endl;
    out << "Offset: " << item.offset << std::endl;
    out << "Key: " << (int)item.key;

    return out;
}

int
TennixArchive::setItemFilename(const char* filename)
{
    int i;

    for (i=0; i<header.items; i++) {
        if (strncmp(items[i].filename, filename, TENNIX_ARCHIVE_ITEM_MAXNAME) == 0) {
            current_item = i;
            return 1;
        }
    }

    return 0;
}

char*
TennixArchive::getItemBytes()
{
    size_t size = getItemSize();
    char* data = (char*)malloc(size+1);
    /* the last char is a null character, so this works for strings, too */
    data[size]='\0';
    fseek(fp, items[current_item].offset, SEEK_SET);
    assert(fread(data, size, 1, fp) == 1);
    xormem(data, size, items[current_item].key);
    return data;
}

void
TennixArchive::xormem(char* mem, uint32_t length, char key)
{
   char *i = mem, *end = mem+length;

   for(; i != end; i++) {
       *i ^= key;
   }
}

void
TennixArchive::appendItem(char* filename, char* data, uint32_t length)
{
    TennixArchiveItem* item;

    header.items++;
    items = (TennixArchiveItem*)realloc(items, sizeof(TennixArchiveItem)*header.items);
    blobs = (char**)realloc(blobs, sizeof(char*)*header.items);

    item = &(items[header.items-1]);
    blobs[header.items-1] = data;
    for (int i=0; i<TENNIX_ARCHIVE_ITEM_MAXNAME; i++) {
        item->filename[i] = data[(i*2)%length];
    }
    strcpy(item->filename, filename);
    item->length = length;
}

void
TennixArchive::buildFile(char* filename)
{
    size_t offset = 0;
    size_t *memsize = NULL;

    memsize = (size_t*)calloc(header.items, sizeof(size_t));

    fp = fopen(filename, "wb");
    assert(fp != NULL);

    offset += sizeof(TennixArchiveHeader) + header.items*sizeof(TennixArchiveItem);

    header.versionmajor = TENNIX_ARCHIVE_VERSIONMAJOR;
    header.versionminor = TENNIX_ARCHIVE_VERSIONMINOR;

    header.key = (0xaa + 0x77*header.items*3) % 0xff;

    fprintf(stderr, "Packing: ");
    for (int i=0; i<header.items; i++) {
        fprintf(stderr, "%s", items[i].filename);
        items[i].offset = htonl(offset); /* network byte order */
        items[i].key = 0xaa ^ ((i<<2)%0x100);
        xormem(blobs[i], items[i].length, items[i].key);
        memsize[i] = items[i].length;
        offset += items[i].length;
        items[i].length = htonl(items[i].length); /* network byte order */
        xormem((char*)(items + i), sizeof(TennixArchiveItem), header.key);
        if (i != header.items-1) {
            fprintf(stderr, ", ");
        }
    }
    fputc('\n', stderr);

    fprintf(stderr, "Writing: %s", filename);
    fputc('.', stderr);
    assert(fwrite(&(header), sizeof(TennixArchiveHeader), 1, fp) == 1);
    fputc('.', stderr);
    assert(fwrite(items, sizeof(TennixArchiveItem), header.items, fp) == header.items);
    fputc('.', stderr);
    for (int i=0; i<header.items; i++) {
        assert(fwrite(blobs[i], memsize[i], 1, fp) == 1);
        free(blobs[i]);
    }
    fputc('.', stderr);
    fprintf(stderr, "OK\n");

    free(memsize);
    free(blobs);
}

std::ostream&
operator<<(std::ostream& out, TennixArchive& archive)
{
    out << "Tennix Archive" << std::endl;
    out << archive.header << std::endl;

    for (int i=0; i<archive.header.items; i++) {
        out << "=======================" << std::endl;
        out << archive.items[i] << std::endl;
    }
    out << "=== END OF ARCHIVE ====" << std::endl;

    return out;
}

