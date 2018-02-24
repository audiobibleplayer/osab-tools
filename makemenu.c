/*
 * makemenu.c - make menu.mnu from audio files for
 *              OSAB - the Open Source Audio Bible player
 *
 * Copyright (C) 2001-2018 Steven R. Patrick (http://audiobibleplayer.org)
 *
 * Parts of this code are derived from VLSI's audio book sample code - http://vlsi.fi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <dirent.h>

/* Max characters in base filename (without extension) */
#define FNAME_MAX 10

/* Max characters in path to menu.mnu */
//#define PATH_MAX 4096 // defined in linux/limits.h

#define MENU_FILE_NAME      "menu.mnu"

#define EXIT_OK             0
#define ERROR_FILE          1
#define ERROR_MEM           2
#define ERROR_PATH_LENGTH   3
#define ERROR_OPTIONS       4

#define FALSE 0
#define TRUE !FALSE

#ifdef WINDOWS
#define SLASH   0x5C
#else
#define SLASH   0x2F
#endif

/* The text string used in Chapter.txt to define the beginning of a new book */
static const char bookmark[] = "BOOK";

/* The text string used in Chapter.txt to define the beginning of a testament */
static const char testamentmark[] = "TESTAMENT";

struct MENUENTRY {
    uint16_t parent;
    uint16_t subtree;
};

/***************************** Global Variables *****************************/
/* testament is a pointer to an array of pointers.  Each of the latter pointers
 * point to an array of u_ints that list the number of chapters per book
 * in the 'testament'.  The first element in the array is the number of books
 * in the 'testament'.
 */
uint16_t **testament;
int testament_count = -1;

/********************************* Functions ********************************/

void error_handler(char *function, int error_num) {
    fprintf(stdout, "Error %d in function %s\r\n", error_num, function);
}


unsigned int filesize(char* filename) {
    struct stat status;

    if (stat(filename, &status) == 0) {
        return status.st_size;
    } else {
        error_handler("filesize", errno);
        return 0;
    }
}


/* Returns total size in bytes of all files listed in chapters_file.
 * Builds **testament array dynamically.
 */ 
unsigned long read_chapter_file(char* chapters_file, uint32_t* unopenable_files) {
    FILE* chapter_txt_file;
    char speech_filename[PATH_MAX];
    int i, book_count = 0, chapter_count = 0;
    unsigned int file_size;
    unsigned long total_files_size = 0;
    void *p;

    chapter_txt_file = fopen(chapters_file, "r");
    if (chapter_txt_file == NULL) {
        fprintf(stdout, "ERROR: cannot open file named %s\r\n", chapters_file);
        exit(ERROR_FILE);
    }

    testament = (void *) malloc(sizeof(size_t));

    while (fgets(speech_filename, PATH_MAX, chapter_txt_file)) {
        if (speech_filename[0] != '#' && ! isspace(speech_filename[0])) {
            if (strncmp(speech_filename, bookmark, strlen(bookmark))) {
                if (strncmp(speech_filename, testamentmark, strlen(testamentmark))) {
                    i = 0;
                    while (speech_filename[++i]) {
                        if (speech_filename[i] == 0x0a) {/* get rid of newline */
                            speech_filename[i] = 0x00;
                        }
                    }
                    FILE* src_file = fopen(speech_filename, "rb");
                    if (src_file) {
                        fclose(src_file);
                        src_file = NULL;
                        printf("sizing %s\r\n", speech_filename);
                        file_size = filesize(speech_filename);
                        if (file_size) {
                            chapter_count++;
                            total_files_size += file_size;
                            *(testament[testament_count] + book_count) = chapter_count;
                        }
                    } else {
                        fprintf(stdout, "ERROR: cannot open source file %s\n", speech_filename);
                        (*unopenable_files)++;
                    }
                } else {
                    testament_count++;
                    p = (void *) realloc(testament, (testament_count + 1) * sizeof(size_t));
                    if (!p) {
                        error_handler("Failed to reallocate memory for testament", 1);
                        free(testament);
                        exit(ERROR_MEM);
                    } else {
                        testament = p;
                    }
                    testament[testament_count] = (uint16_t *) malloc(sizeof(uint16_t));
                    book_count = 0;
                }
            } else {
                book_count++;
                *testament[testament_count] = book_count;
                p = (uint16_t *) realloc(testament[testament_count], (book_count + 1) * sizeof(uint16_t));
                if (!p) {
                    error_handler("Failed to reallocate memory for testament[testament_count].", 2);
                    free(testament[testament_count]);
                    exit(ERROR_MEM);
                } else {
                    testament[testament_count] = p;
                }

                chapter_count = 0;
            }
        }
    }

    fclose(chapter_txt_file);
    return total_files_size;
}


int print_testament_array() {
    int tst, book;

    for (tst = 0; tst <= testament_count; tst++) {
        for (book = 0; book <= *testament[tst]; book++) {
            printf("%d ", *(testament[tst] + book));
        }
        printf("\n");
    } 
}


/* Returns number of files (chapters) listed
 * in chapter.txt file
 */
int num_chaps() {
    int tst, book, chaps = 0;

    for (tst = 0; tst <= testament_count; tst++) {
        for (book = 1; book <= *testament[tst]; book++) {
            chaps += *(testament[tst] + book);
        }
    } 
    return chaps;
}


int entries_in_menu(void) {
    int tst, book, entries = 0;

    for (tst = 0; tst <= testament_count; tst++) {
        entries++;
        for (book = 0; book <= *testament[tst]; book++) {
            entries += *(testament[tst] + book);
        }
    }
    printf("Number of menu entries required = %d\n", entries);
    return entries;
}


/* Returns size of menu in bytes.
 * Returns 0 if cannot allocate memory.
 */
int mkmnu(struct MENUENTRY *menu) {
    int tst, book, chp, entries, entry, index;

    entries = entries_in_menu();

    printf("\nentry\tparent\tsubtree\n");
    printf("Testaments\n");

    for (tst = 0, entry = 0, index = testament_count + 1; tst <= testament_count; tst++, entry++) {
        menu[entry].parent = 0;
        menu[entry].subtree = __builtin_bswap16(index);
        printf("%d\t%d\t%d\n", entry, menu[entry].parent, menu[entry].subtree);
        index += *testament[tst];
    }

    printf("\n");
    printf("Books\n");

    for (tst = 0; tst <= testament_count; tst++) {
        for (book = 1; book <= *testament[tst]; book++, entry++) {
            menu[entry].parent = __builtin_bswap16(tst);
            menu[entry].subtree = __builtin_bswap16(index);
            printf("%d\t%d\t%d\n", entry, menu[entry].parent, menu[entry].subtree);
            index += *(testament[tst] + book);
        }
    }

    printf("\n");
    printf("Chapters\n");

    for (tst = 0; tst <= testament_count; tst++) {
        for (book = 0; book < *testament[tst]; book++) {
            for (chp = 0; chp < *(testament[tst] + book + 1); chp++, entry++) {
                menu[entry].parent = __builtin_bswap16(__builtin_bswap16(menu[tst].subtree) + book);
                menu[entry].subtree = 0;
                printf("%d\t%d\t%d\n", entry, menu[entry].parent, menu[entry].subtree);
            }
        }
    }

    return sizeof(struct MENUENTRY) * entries;
}


void touch_files(unsigned int num_files, char *mnt_path) {
    unsigned int chap;
    char path_to_file[PATH_MAX], filename[FNAME_MAX];
    FILE *chap_file;

    for (chap = 1; chap <= num_files; chap++) {
        memset(path_to_file, 0, PATH_MAX);
        strncpy(path_to_file, mnt_path, PATH_MAX);
        snprintf(filename, FNAME_MAX, "%08d", chap);
        strncat(path_to_file, filename, FNAME_MAX);
        strncat(path_to_file, ".ogg", 5);
        chap_file = fopen(path_to_file, "wbx");
        if (NULL == chap_file) {
            fprintf(stdout, "Cannot create file %s - ", path_to_file);
            exit(ERROR_FILE);
        }
        fclose(chap_file);
    }
}


void copy_files(char *chapter_file, char *mnt_path) {
    char dst_fname[PATH_MAX];
    char src_fname[PATH_MAX];
    char filename[FNAME_MAX];
    FILE *chap_file, *dst_file, *src_file;
    unsigned int i, chap = 1, file_size;
    void *file_buffer = NULL;

    chap_file = fopen(chapter_file, "r");
    if (NULL == chap_file) {
        fprintf(stdout, "ERROR, cannot open chapter file %s\n", chapter_file);
        exit(ERROR_FILE);
    }

    while (fgets(src_fname, PATH_MAX, chap_file)) {
        if (src_fname[0] != '#' && ! isspace(src_fname[0])) {
            if (strncmp(src_fname, bookmark, strlen(bookmark))) {
                if (strncmp(src_fname, testamentmark, strlen(testamentmark))) {
                    i = 0;
                    while (src_fname[++i]) {
                        if ((src_fname[i] == 0x0a) || (src_fname[i] == 0x0d)) {
                            src_fname[i] = 0;
                        }
                    }
                    memset(dst_fname, 0, PATH_MAX);
                    strncpy(dst_fname, mnt_path, PATH_MAX);
                    snprintf(filename, FNAME_MAX, "%08d", chap++);
                    strncat(dst_fname, filename, FNAME_MAX);
                    strncat(dst_fname, ".ogg", 5);
                    printf("Copying from %s to %s\n", src_fname, dst_fname);
                    dst_file = fopen(dst_fname, "wb");
                    if (NULL == dst_file) {
                        fprintf(stdout, "\nERROR: cannot open destination file %s\a\n\n", dst_fname);
                        exit(ERROR_FILE);
                    }
                    src_file = fopen(src_fname, "rb");
                    if (NULL == src_file) {
                        fprintf(stdout, "\nERROR: cannot open source file %s\a\n\n", src_fname);
                        exit(ERROR_FILE);
                    }
                    fseek(src_file, 0, SEEK_END);
                    file_size = ftell(src_file);
                    rewind(src_file);
                    file_buffer = malloc(file_size);
                    if (NULL == file_buffer) {
                        fprintf(stdout, "\nERROR: cannot allocate memory for file buffer\a\n\n");
                        exit(ERROR_MEM);
                    }
                    fread(file_buffer, 1, file_size, src_file);
                    fwrite(file_buffer, 1, file_size, dst_file);
                    if (file_buffer) free(file_buffer);
                    file_buffer = NULL;
                    if (src_file) fclose(src_file);
                    src_file = NULL;
                    if (dst_file) fclose(dst_file);
                    dst_file = NULL;
                }
            }
        }
    }

    if (chap_file) fclose(chap_file);
}


void check_chapters(char* chapter_file) {
    unsigned long tot_size;
    uint32_t unopenable_files = 0;
    tot_size = read_chapter_file(chapter_file, &unopenable_files);
    if (unopenable_files) {
        fprintf(stdout, "\nERROR: there are %d files listed in %s that cannot be opened for reading.\a\n\n", unopenable_files, chapter_file);
        exit(ERROR_FILE);
    }
}


void upload(char* mnt_path, char* chapter_file) {
    size_t path_length;
    char menu_path[PATH_MAX] = { 0 };
    struct MENUENTRY *menu;
    int tst, menu_size;
    FILE *menu_file;

    check_chapters(chapter_file);

    path_length = strnlen(mnt_path, PATH_MAX);
    if (path_length > PATH_MAX - strlen(MENU_FILE_NAME) - 3) {
      fprintf(stdout, "\nERROR: Path length for mount point is too long.");
      exit(ERROR_PATH_LENGTH);
    }

    if (mnt_path[path_length - 1] != SLASH) {
        mnt_path[path_length] = SLASH;
        mnt_path[path_length + 1] = 0;
    }

    strncpy(menu_path, mnt_path, PATH_MAX);
    strncat(menu_path, MENU_FILE_NAME, PATH_MAX);

    {   /*create dir when it does not exist*/
        DIR *dirh;
        dirh = opendir(mnt_path);
        if (dirh == NULL) {
#ifdef WINDOWS
            if(mkdir(mnt_path) == -1) {
#else
            if(mkdir(mnt_path, S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
#endif
                printf("\nERROR: Cannot create directory %s\n", mnt_path);
                exit(ERROR_FILE);
            }
        }
        else closedir(dirh);
    }

    menu = malloc(sizeof(struct MENUENTRY) * entries_in_menu());
    if (NULL == menu) {
        fprintf(stdout, "\nERROR: cannot allocate memory for menu.\n");
        exit(ERROR_MEM);
    }

    menu_size = mkmnu(menu);
    printf("Size of MENU_FILE_NAME is %d\n", menu_size);

    menu_file = fopen(menu_path, "wb");
    if (menu_file == NULL) {    
        fprintf(stdout, "\nERROR: cannot open %s\n", menu_path);
        exit(ERROR_FILE);
    }
    fclose(menu_file);

    printf("Number of chapter files is %d\n", num_chaps());
    touch_files(num_chaps(), mnt_path);

    menu_file = fopen(menu_path, "wb");
    fwrite(menu, 1, menu_size, menu_file);
    fclose(menu_file);

    copy_files(chapter_file, mnt_path);

    printf("Done copying files to SD card.\n");

    if (menu) free(menu);

    for (tst = 0; tst <= testament_count; tst++)
    {
        if(testament[tst]) free(testament[tst]);
    }
    if (testament) free(testament);
}


/*********************************** Main ***********************************/
int main(int argc, char *argv[]) {
    char mnt_path[PATH_MAX] = { 0 };
    char chapter_file[PATH_MAX] = { 0 };
    int option;

    const char *usage = "Usage... \nUpload with:\nmakemenu -c <chapter_file> -m <mount_point> -u\n\nVerify with:\nmakemenu -c <chapter_file> -m <mount_point> -v\n";

    if (argc < 2) {
        fprintf(stdout, usage);
        exit(EXIT_OK);
    }

	  while ((option = getopt(argc, argv, "bc:m:puv")) != -1) {
      switch(option) {
        case 'b':
          printf("Build date: %s\n", __DATE__);
          break;

        case 'c':
          snprintf(chapter_file, PATH_MAX, optarg);
          printf("chapter_file = %s\n", chapter_file);
          break;

        case 'm':
          snprintf(mnt_path, PATH_MAX, optarg);
          printf("mount_point = %s\n", mnt_path);
          break;

        case 'p':
          print_testament_array();
          break;

        case 'u':
          if (strnlen(mnt_path, PATH_MAX) == 0) {
            fprintf(stdout, "\nERROR: -m option missing.");
            exit(ERROR_OPTIONS);
          }
          if (strnlen(chapter_file, PATH_MAX) == 0) {
            fprintf(stdout, "\nERROR: -c option missing.");
            exit(ERROR_OPTIONS);
          }
          upload(mnt_path, chapter_file);
          break;

        case 'v':
          break;

        case 'h':
        default:
          fprintf(stdout, usage);
          exit(EXIT_OK);
      }
    }

    exit(EXIT_OK);
}

