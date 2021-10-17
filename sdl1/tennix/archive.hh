#ifndef __ARCHIVE_HH
#define __ARCHIVE_HH

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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TENNIX_ARCHIVE_HEADER "thpinfo.com/2009/tennix/afmt "
#define TENNIX_ARCHIVE_HEADER_LEN 30

#define TENNIX_ARCHIVE_VERSIONMAJOR 1
#define TENNIX_ARCHIVE_VERSIONMINOR 0

#define TENNIX_ARCHIVE_ITEM_MAXNAME 86


/* architecture-independent (in-file) structs */

struct _TennixArchiveItem {
    char filename[TENNIX_ARCHIVE_ITEM_MAXNAME];
    uint32_t offset; /* network byte order */
    uint32_t length; /* network byte order */
    uint8_t key;
};

typedef struct _TennixArchiveItem TennixArchiveItem;

std::ostream&
operator<<(std::ostream& out, TennixArchiveItem& item);

struct _TennixArchiveHeader {
    char header[TENNIX_ARCHIVE_HEADER_LEN];
    uint8_t versionmajor; /* major file version */
    uint8_t versionminor; /* minor file version */
    uint8_t key;
    uint8_t items; /* maximum 255 files per archive */
};

typedef struct _TennixArchiveHeader TennixArchiveHeader;

std::ostream&
operator<<(std::ostream& out, TennixArchiveHeader& header);

class TennixArchive {
    private:
        FILE* fp;
        TennixArchiveHeader header;
        TennixArchiveItem* items;
        char** blobs;
        size_t offset;
        int current_item;
        int building;

        static void xormem(char* mem, uint32_t length, char key);

    public:
        TennixArchive() {
            strcpy(header.header, TENNIX_ARCHIVE_HEADER);
            header.items = 0;
            building = 1;
            fp = NULL;
        }

        TennixArchive(const char* filename, const char* fallback=NULL);

        ~TennixArchive() {
            if (fp != NULL) {
                fclose(fp);
            }
            free(items);
        }

        int setItemFilename(const char* filename);

        const char* getItemFilename() {
            return items[current_item].filename;
        }

        char* getItemBytes();

        size_t getItemSize() {
            return items[current_item].length;
        }

        int endOfFile() {
            return current_item >= header.items;
        }

        void next() {
            current_item++;
        }

        /* only for building/utility mode: */
        void appendItem(char* filename, char* data, uint32_t length);
        void buildFile(char* filename);

        friend std::ostream& operator<<(std::ostream& out, TennixArchive& archive);
};

/* Used for dumping the contents of a TennixArchive */
std::ostream&
operator<<(std::ostream& out, TennixArchive& archive);

#endif
