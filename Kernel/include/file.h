#ifndef FILE_H
#define FILE_H

#include <stdint.h>


#ifndef EOF
#define EOF -1
#endif

#ifndef MAX_NAME
#define MAX_NAME 16
#endif

#ifndef FILE_FLAGS
#define F_READ 0x01
#define W_WRITE 0x02
#endif



typedef enum {INPUT = 0, OUTPUT} IOOperation;
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