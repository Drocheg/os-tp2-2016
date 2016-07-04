#include <stdlib.h>
#include <stddef.h>

//Parses the specified value to string, returning the number of digits converted.
uint32_t intToStr(uint64_t value, char * buffer) {
	return intToStrBase(value, buffer, 10);
}

//Parses the specified value to string using the specified base, returning the number of digits converted.
uint32_t intToStrBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}



void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i = 0;

	if ((uint64_t)destination % sizeof(uint32_t) == 0
		&& (uint64_t)source % sizeof(uint32_t) == 0
		&& length % sizeof(uint32_t) == 0) {

		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++) {
			d[i] = s[i];
		}
	} else {
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++) {
			d[i] = s[i];
		}
	}

	return destination;
}

//Compares string equality
uint8_t streql(const char *s1, const char *s2) {
	while(*s1 != 0 && *s2 != 0) {
		if(*s1 != *s2) return 0;
		s1++;
		s2++;
	}
	return *s1 == *s2;	//Ensure equal length
}

//Compares two strings lexicographically
int32_t strcmp(const char *s1, const char *s2) {
	while(*s1 != 0 && *s2 != 0) {
		int diff = *s1 - *s2;
		if(diff != 0) return diff;
		s1++;
		s2++;
	}
	return *s1-*s2;
}

//Calculates length of a (null-terminated) string
uint64_t strlen(const char *s) {
	
	uint64_t count = 0;
	if (s == NULL) {
		return -1;
	}
	while (s[count] != 0) {
		count++;
	}
	return count;

}

//Returns the index of the first occurrence of the specified "needle" string
//in the specified "haystack" string
int32_t indexOf(const char *needle, const char *haystack) {
	int hIndex, nIndex, nLen = strlen(needle);
	for(hIndex = 0, nIndex = 0; haystack[hIndex] != 0; hIndex++) {
		if(hIndex >= nLen) return -1;
		if(haystack[hIndex] == needle[nIndex]) {
			nIndex++;
			if(nIndex == nLen-1) {
				return hIndex-nLen;
			}
		}
		else {
			nIndex = 0;
		}
	}
	return -1;
}

//Creates a substring of the specified source string in the specified destination
//string, starting and ending at the specified indices (end index exclusive)
void substr(const char *src, char *dest, uint32_t start, uint32_t end) {
	int len = strlen(src), i;
	for(i = start; i < end && i < len-1; i++) {
		dest[i] = src[i];
	}
	dest[i] = 0;
}

char * toUpperStr(char *s) {
	int i;
	for(i = 0; *s != 0; s++) {
		s[i] = toUpperChar(s[i]);
	}
	return s;
}

char * toLowerStr(char *s) {
	int i;
	for(i = 0; *s != 0; s++) {
		s[i] = toLowerChar(s[i]);
	}
	return s;
}

char toUpperChar(char s) {
	if(s >= 'a' && s <= 'z') return s-32;
	else return s;
}

char toLowerChar(char s) {
	if(s >= 'A' && s <= 'Z') return s+32;
	else return s;
}

uint8_t isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

uint8_t isNumber(char c) {
	return c >= '0' && c <= '9';
}