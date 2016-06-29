#ifndef PROCESS_H
#define PROCESS_H

#include <file.h>
#include <stdint.h>
#include <processCommon.h>


#ifndef MAX_FILES
#define MAX_FILES 16
#endif

// #ifndef MAX_NAME_LENGTH
// #define MAX_NAME_LENGTH 32
// #endif


uint64_t initializePCB();

uint64_t createProcess(uint64_t parentPid, char name[MAX_NAME_LENGTH], void *entryPoint, uint64_t argc, char *argv[]);

uint64_t getProcessPID(uint64_t PCBIndex);
uint64_t getProcessParentPID(uint64_t PCBIndex);
char *getProcessName(uint64_t PCBIndex);
void *getProcessStack(uint64_t PCBIndex);
uint64_t getFilesQuantity(uint64_t PCBIndex);
uint64_t getFileDescriptor(uint64_t PCBIndex, File file);
uint64_t getFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor);
File getFile(uint64_t PCBIndex, uint64_t fileDescriptor);
uint64_t setProcessStack(uint64_t PCBIndex, void *stack);
uint64_t setFileFlags(uint64_t PCBIndex, uint64_t fileDescriptor, uint64_t flags);

uint64_t destroyProcess(uint64_t PCBIndex);

void *malloc(uint64_t PCBIndex, int64_t size);






#endif /* PROCESS_H */