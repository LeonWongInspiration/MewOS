#include "FAT.h"

void readFAT12(int *fat, unsigned char *img){
    int i;
    int j = 0;
    for (i = 0; i < 2880; i += 2) {
        fat[i] = (img[j] | img[j + 1] << 8) & 0xfff;
        fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
        j += 3;
    }
}

void loadFile(int cluster, int size, char *buf, int *fat, char *img){
    int i;
    while (1) {
        if (size <= 512) {
            for (i = 0; i < size; ++i) {
                buf[i] = img[cluster * 512 + i];
            }
            break;
        }
        for (i = 0; i < 512; ++i) {
            buf[i] = img[cluster * 512 + i];
        }
        size -= 512;
        buf += 512;
        cluster = fat[cluster];
    }
}

FAT12 *searchFile(char *name, FAT12 *file, int max){
    int i = 0;
    int j = 0;
    char s[12] = "            ";
    // First set the name of the file in the format of FAT12 without case.
    for (i = 0; name[i] != '\0'; ++i) {
        if (j >= 11) {
            return NULL;
        }
        if (name[i] == '.' && j <= 8) {
            j = 8;
        }
        else {
            s[j] = name[i];
            if (s[j] >= 'a' && s[j] <= 'z') {
                s[j] -= 'a' - 'A';
            }
            ++j;
        }
    }
    // Then search the file list if there are any files with the same name.
    for (i = 0; i < max; ++i) {
        if (file[i].name[0] == 0x00) { // This is not a file.
            continue;
        }
        if ((file[i].type & 0x18) == 0) {
            char flag = 1;
            for (j = 0; j < 11; ++j) {
                if (file[i].name[j] != s[j]) {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1) {
                return file + i;
            }
        }
    }
    return NULL;
}
