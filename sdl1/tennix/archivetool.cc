
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "archive.hh"
#include <libgen.h>

int main(int argc, char* argv[])
{
    TennixArchive* tnxar;
    char* data;
    FILE *fp;
    const char* filename;
    char *bn = (char*)basename(argv[0]);
    int len, i;
    struct stat st;

    if(strcmp(bn, "archive") == 0) {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s archive.tnx file1 [....]\n", bn);
            exit(EXIT_FAILURE);
        } else if (argc == 2) {
            fprintf(stderr, "Refusing to create an empty archive.\n");
            exit(EXIT_FAILURE);
        }

        if (stat(argv[1], &st) != -1) {
            fprintf(stderr, "File %s already exists. Aborting.\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        tnxar = new TennixArchive();

        fprintf(stderr, "Creating %s with %d files\n", argv[1], argc-2);
        for (i=2; i<argc; i++) {
            fp = fopen(argv[i], "rb");
            fseek(fp, 0, SEEK_END);
            len = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            data = (char*)malloc(len);
            assert(fread(data, len, 1, fp) == 1);
            fclose(fp);
            tnxar->appendItem((char*)basename(argv[i]), data, len);
        }
        tnxar->buildFile(argv[1]);
        delete tnxar;
    } else if(strcmp(bn, "dump") == 0) {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s archive.tnx\n", bn);
            exit(EXIT_FAILURE);
        }
        TennixArchive archive(argv[1]);
        std::cerr << archive << std::endl;
    } else if(strcmp(bn, "extract") == 0) {
        if (argc < 2 || argc > 3) {
            fprintf(stderr, "Usage: %s archive.tnx [file]\n", bn);
            exit(EXIT_FAILURE);
        }
        tnxar = new TennixArchive(argv[1]);
        if (argc == 2) {
            while (!tnxar->endOfFile()) {
                filename = tnxar->getItemFilename();
                data = tnxar->getItemBytes();
                len = tnxar->getItemSize();
                fprintf(stderr, "Extracting: %s", filename);
                fprintf(stderr, " (%d bytes)", len);
                fp = fopen(filename, "wb");
                fputc('.', stderr);
                assert(fwrite(data, len, 1, fp) == 1);
                fputc('.', stderr);
                fclose(fp);
                fprintf(stderr, ".OK\n");
                free(data);
                tnxar->next();
            }
        } else if (argc == 3) {
            filename = argv[2];
            if (tnxar->setItemFilename(filename) != 0) {
                fprintf(stderr, "Extracting: %s", filename);
                data = tnxar->getItemBytes();
                len = tnxar->getItemSize();
                fprintf(stderr, " (%d bytes)", len);
                fp = fopen(filename, "wb");
                fputc('.', stderr);
                assert(fwrite(data, len, 1, fp) == 1);
                fputc('.', stderr);
                fclose(fp);
                fprintf(stderr, ".OK\n");
                free(data);
            } else {
                fprintf(stderr, "File not found in %s: %s\n", argv[1], filename);
                delete tnxar;
                exit(EXIT_FAILURE);
            }
        }
        delete tnxar;
    }

    return EXIT_SUCCESS;
}

