#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <file-common.h>




void putchar(char c) {
	write(STDOUT_, &c, 1);
}

char getchar() {
	char result;
	read(STDIN_, &result, 1);
	return result;
}






/*
 * Gets a line (i.e. a string finishing in '\n') from STDIN
 * Returns the string length
 *
 * Note: it doesn't include the '\n' character in the string, but it null-terminates it
 */
int64_t readLine(char *buffer, uint64_t buffSize){

	uint64_t i = 0;
	char c;

	while ( ((c = getchar()) != '\n') && (i < (buffSize - 1)) ) {
		buffer[i++] = c;
	}
	buffer[i] = 0; /* NULL-terminates the string */
	if (c != '\n') {
		while (getchar() != '\n'); /* Cleans stdin buffer */
	}

	return (i - 1);
}





// void printf(char * fmt, ...) {

// 	va_list ap;
//     int i, j = 0, flag = 0; // Manages the format string parse
//     char result[256]; // local buffer
	
// 	va_start(ap, fmt);
//     for (i = 0 ; fmt[i] != 0 ; i++) {
//         if (flag) {
//         	switch (fmt[i]) {
                
//                 case 'd': {
//                     int num, figures;
//                     num = va_arg(ap, int);
//                     figures = decimalDigits(num);
                    
//                     while (figures) {
                        
//                         result[j++] = (char) ((num % ipow(10, figures)) / ipow(10, figures - 1)) + '0';
//                         figures--;
//                         if (j == 256) {
//                         	write(1, (uint64_t)result, 256);
//                         	j = 0;
//                         }

//                     }
//                     break;
//                 }

//                 case 's': {
//                     char * aux;
//                     aux = va_arg(ap, char *);
//                     int k = 0;
//                     while (aux[k] != 0) {

//                         result[j++] = aux[k++];
//                         if (j == 256) {
//                         	write(1, (uint64_t)result, 256);
//                         	j = 0;
//                         }
//                     }
//                     break;
//                 }
//                 default: {
//                 	const char * error = SYMBOL_ERROR;
//                 	write(1, (uint64_t)error, sizeof(SYMBOL_ERROR));
//                 	break;
//                 }
                    
//             }
//             flag = 0;
            
//         } else {
//             if (fmt[i] == '%') {
//                 flag = 1;
//             } else {
//                 result[j++] = fmt[i];
//                 if (j == 256) {
//                 	write(1, (uint64_t)result, 256));
//                 	j = 0;
//                 }
//             }
//         }
//     }
//     va_end(ap);
//     write(1, (uint64_t)result, (uint64_t) (j % 256));
// }
