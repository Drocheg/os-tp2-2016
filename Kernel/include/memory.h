#ifndef MEMORY_H
#define MEMORY_H


#ifndef PAGE_SIZE
#define PAGE_SIZE 0x2000
#endif

typedef enum {PUSH_PAGE = 1, POP_PAGE} Action;

void initializePageStack();
void pageManager(Action action, void **page);



#endif /* MEMORY_H */