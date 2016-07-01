#ifndef BASICFILE_H
#define BASICFILE_H

#include <stdint.h>

/*
* Basic File - a circular byte array.
*/

typedef struct basicFile_s * BasicFile;

/*
* Memory space where the File resides. For now, valid values are:
* - Kernel (only kernel should access this File)
* - Heap (may be part of a userland program)
*/
typedef enum {KERNEL = 1, HEAP} Place;

/**
* Creates a File table in kernel memory to track and manage basic files.
*
* @return 0 on success, -1 otherwise.
*/
uint64_t initializeBasicFiles();

/**
* Creates a bascic file with the specified information.
*
* @param name The File name. May not exceed MAX_NAME characters.
* @param stream Memory address where data will actually be read from/written to.
* @param size The File size. In other words, the highest addressable offset from
* 			  <stream> that is considered valid for this File.
* @param place Where this File resides. See typedef above.
* @param indexManager File index manager. See typedef above.
* @return The created File, or NULL on error.
*/
BasicFile createBasicFile(const char* name, void *stream, uint64_t size, Place place);

/**
* Creates a File of size PAGE_SIZE, in the heap, with a basic index manager.
*
* @param name The File name. May not exceed MAX_NAME characters.
* @return The created File, or NULL on error.
*/
BasicFile createBasicFileWithName(const char* name);

/**
* Destroys the specified File. If the File was stored in heap, that memory is now free
* to use.
*
* @param file the File to destroy.
* @return 1 on success, -1 on error.
*/
int destroyBasicFile(BasicFile file);

/**
* Reads a single byte from the specified File. Does NOT handle blocking processes
* if there is no data to read. Use a system call (which uses file descriptors,
* not Files) for that behavior.
*
* @return The read character, or EOF if there is no data to read or an error ocurred.
*/
int basicFileReadChar(BasicFile file);

/**
* Writes a single byte to the specified File. Does NOT handle blocking processes
* if the File is full. Use a system call (which uses file descriptors, not Files)
* for that behavior.
*
* @return 1 on success, or EOF if the File is full or an error ocurred.
*/
int basicFileWriteChar(char c, BasicFile file);

/**
* Checks whether the specified File is full. If a File is full, write operations
* may block or fail.
*
* @return 1 if this File is full, 0 otherwise.
*/
int basicFileIsFull(BasicFile file);

/**
* Checks whether the specified File if empty. If a File is empty, read operations
* may block or fail.
*
* @return A positive value if data is available, 0 otherwise.
*/
int basicFileIsEmpty(BasicFile file);

/**
* Gets the specified File's name, which identifies it.
*
* @return A char[MAX_FILE] with the specified File's name.
*/
char* getBasicFileName(BasicFile f);

uint64_t getBasicFileSize(BasicFile f);

/**
* Finds a File with the specified name.
*
* @return The matching File, or NULL if not found.
*/
BasicFile findBasicFile(const char* name);

#endif