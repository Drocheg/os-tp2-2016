#ifndef RAW_KEYS_H
#define RAW_KEYS_H







int64_t rawAddElement(uint8_t scanCode);


/* File Management */
int8_t rawReadChar(uint64_t index, char *dest);
int8_t rawWriteChar(uint64_t index, char *src);
int8_t rawIsFull(uint64_t index);
int8_t rawIsEmpty(uint64_t index);
int8_t rawClose(uint64_t index);







#endif /* RAW_KEYS_H */