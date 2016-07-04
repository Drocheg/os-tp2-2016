#ifndef UNISTD_H
#define UNISTD_H



void read(uint8_t fd, char *buff, uint64_t maxBytes);

void write(uint8_t fd, char *buff, uint64_t maxBytes);



void reboot_sys();

void scroll_sys();

void sleep_sys(uint64_t miliseconds);

#endif  /* UNISTD_H */