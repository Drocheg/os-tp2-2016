#ifndef FILE_H
#define FILE_H

#include <stdint.h>


#define EOF -1

#define MAX_NAME 16







typedef enum {KERNEL = 1, HEAP} Place;
typedef struct file_s * File;

typedef void (*IndexManager)(uint64_t *, uint64_t);

uint64_t initializeFileSystem();

File createFileWithName(char name[MAX_NAME]);
File createFile(char name[MAX_NAME], void *stream, uint64_t maxSize, Place place, IndexManager indexManager);
File searchForFileByName(char name[MAX_NAME]);
uint64_t readChar(File file);
uint64_t writeChar(char c, File file);
uint64_t dataAvailable(File file);
uint64_t hasFreeSpace(File file);
uint64_t destroyFile(File file);


void basicIndexManager(uint64_t *index, uint64_t maxSize);


#endif /* FILE_H */