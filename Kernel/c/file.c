#include <memory.h>
#include <file.h>
#include <stdlib.h>
#include <stddef.h>
#include <kernel-lib.h>



struct file_s {
	
	char name[MAX_NAME];			/* Stores the file name */
	uint64_t maxSize;				/* Stores the max amount of bytes the file can hold */
	void *stream;					/* Stores a pointer to the stream of the file */
	uint32_t place;					/* Stores if stream is a memory page, or a kernel buffer (for example, video buffer) */
	IndexManager indexManager;		/* Stores the function that update indexes  */
	uint64_t enqueueIndex;			/* Stores the next position to put an element into the file */
	uint64_t dequeueIndex;			/* Stores the position to get an element from the file */
	uint32_t unreadData;			/* Stores if there is data available for read */
};



/* Static variables */
static void *filesTablePage;
static uint64_t maxFiles = PAGE_SIZE / sizeof(struct file_s); /* Should be 0x80 */
static File filesTable; /* Easiear to access an entry */
static uint64_t filesQuantity = 0;



/* Static functions prototypes */
static uint64_t checkFileCreationParams(char name[MAX_NAME], void *memory, uint64_t maxSize, Place place);
static uint64_t checkFilePointer(File file);


/*
 * Initializes the "file system"
 * Returns 0 on success, or -1 otherwise
 */
uint64_t initializeFileSystem() {

	pageManager(POP_PAGE, &filesTablePage);
	if (filesTablePage == NULL) {
		return -1; /* Couldn't start file system */
	}

	filesTable = (File) filesTablePage;
	memset(filesTable, 0, PAGE_SIZE); /* Clears file table */
	return 0;
}


File createFileWithName(char name[MAX_NAME]) {

	void *stream;
	File newFile;
	pageManager(POP_PAGE, &stream);
	if (stream == NULL) {
		return NULL;
	}

	newFile = createFile(name, stream, PAGE_SIZE, HEAP, &basicIndexManager);

	if (newFile == NULL) {
		pageManager(PUSH_PAGE, &stream);
	}
	return newFile;


}


File createFile(char name[MAX_NAME], void *stream, uint64_t maxSize, Place place, IndexManager indexManager) {

	File newFile = NULL;
	uint64_t index = 0;
	if (filesQuantity >= maxFiles) {
		return NULL;
	}

	if (checkFileCreationParams(name, stream, maxSize, place)) {
		return NULL;
	}


	/* Searches for the next available entry */
	while ( ((filesTable[index]).name)[0] != 0) { /* Avoids calling strcmp(name, "") */
		index++;
	}
	/* Index won't never be greater or equal to maxFiles */

	newFile = &(filesTable[index]);
	memcpy((void *) newFile->name, name, MAX_NAME);
	newFile->maxSize = maxSize;
	newFile->stream = stream;
	newFile->place = (uint32_t) place;
	newFile->indexManager = indexManager;
	newFile->enqueueIndex = 0;
	newFile->dequeueIndex = 0;
	newFile->unreadData = 0;
	
	return newFile;
}

File searchForFileByName(char name[MAX_NAME]) {

	uint64_t index = 0;
	while (index < maxFiles && strcmp((filesTable[index]).name, name)){
		index++;
	}
	if (index == maxFiles) {
		return NULL;
	}
	return &(filesTable[index]);
}






uint64_t readChar(File file) {

	char *stream;
	char c;
	if (checkFilePointer(file)) {
		return EOF;
	}

	if (file->enqueueIndex == file->dequeueIndex ) {
		return EOF;
	}
	stream = (char *)file->stream;
	c = stream[file->dequeueIndex];
	(file->indexManager)(&(file->dequeueIndex), file->maxSize);
	file->unreadData = (file->enqueueIndex == file->dequeueIndex) ? 0 : 1;
	return (uint64_t) c;
}


uint64_t writeChar(char c, File file) {

	char *stream;
	if (checkFilePointer(file)) {
		return EOF;
	}

	if ( !hasFreeSpace(file) ) {
		return EOF;
	}
	stream = (char *)file->stream;
	stream[file->enqueueIndex] = c;
	(file->indexManager)(&(file->enqueueIndex), file->maxSize);
	file->unreadData = 1;
	return (uint64_t) c;
}

/*
 * Returns 1 if there is data available, or 0 otherwise
 */
uint64_t dataAvailable(File file) {

	return file->unreadData; 
}

/*
 * Returns 1 if there is free space, or 0 otherwise
 */
uint64_t hasFreeSpace(File file) {

	return !(file->unreadData) && (file->enqueueIndex == file->dequeueIndex);
}


uint64_t destroyFile(File file) {

	if (checkFilePointer(file)) { /* Must check file pointer in order to avoid buffer overflows */
		return -1; /* Invalid file pointer */
	}

	if ( ((Place)file->place) == HEAP ) {
		pageManager(PUSH_PAGE, &(file->stream)); /* Shouldn't return with errors */
	}

	memset(file, 0, sizeof(struct file_s));
	return 0;

}


void basicIndexManager(uint64_t *index, uint64_t maxSize) {

	if ((*index) == (maxSize - 1)) {
		(*index) = 0;
	} else {
		(*index)++;
	}
	return;
}





static uint64_t checkFileCreationParams(char name[MAX_NAME], void *memory, uint64_t maxSize, Place place) {

	if (name[0] == 0) { /* Avoids calling strcmp(name, "") */
		return -1; /* an empty string is not a valid name for a file */
	}

	if (memory == NULL || maxSize <= 0 || (place != KERNEL && place != HEAP)) {
		return -1; /* Other invalid values */
	}

	if (searchForFileByName(name) != NULL) {
		return -1; /* There is a file with that name already on the table */
	}

	return 0;
}

static uint64_t checkFilePointer(File file) {

	uint64_t entry;
	if (file == NULL) {
		return -1; /* Invalid file poiner (can't be NULL) */
	}

	if ( ((void *)file < (void *)filesTable) ||  ((void *)file >= ( ((void *)filesTable) + PAGE_SIZE)) ) {
		return -1; /* Invalid file pointer (out of table range) */
	}

	entry = (uint64_t) ((void *)file - (void *)filesTable);
	if (entry % sizeof(struct file_s)) {
		return -1; /* Invalid file pointer (not a valid entry) */
	}
	return 0;
}














