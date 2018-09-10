/** MewOS FAT12 File Management
 * 
 * @author: Leon Wong
 * @build: 201809102248 FINAL
 * @brief: This file includes functions on file management with FAT12 drives.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#ifndef FAT_H
#define FAT_H 1

#include "Structs.h"

#include "include/stdio.h"

const static int DISKIMG_ADDRESS = 0x00100000;

/**
 * @brief: Read a FAT12 table from img's address and save cluster numbers to fat array.
 * @param: (int *)fat: The array to save cluster numbers.
 * @param: (uchar *)img: The start address of the FAT table.
 */ 
void readFAT12(int *fat, unsigned char *img);

/**
 * @brief: Read a file in to a buffer.
 * @param: (int)cluster: The initial cluster number of a file.
 * @param: (int)size: The size of a file (in Bytes).
 * @param: (char *)buf: The array to save the file in binary.
 * @param: (int *)fat: The FAT12 table.
 * @param: (char *)img: The address of the image.
 */ 
void loadFile(int cluster, int size, char *buf, int *fat, char *img);

/**
 * @brief: Find a file with its name in a list of files.
 * @param: (char *)name: The name to search.
 * @param: (FAT12 *)file: The array of files.
 * @param: (int)max: The number of files in the list.
 * @warning: This function may return NULL when it cannot find the file!
 * @warning: This function will IGNORE the case of the file name!
 */ 
FAT12 *searchFile(char *name, FAT12 *file, int max);

#endif
