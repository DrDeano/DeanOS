#include <string.h>
#include <stdint.h>

int memcmp(const void * ptr1, const void * ptr2, size_t n) {
	if(ptr1 == ptr2 || n == 0 || !ptr1 || !ptr2) {
		return 0;
	}
	unsigned char * c1 = (unsigned char *) ptr1;
	unsigned char * c2 = (unsigned char *) ptr2;
	
	while(n--) {
		if(*c1 != *c2) {
			return *c1 - *c2;
		} else {
			c1++;
			c2++;
		}
	}
	return 0;
}

void * memcpy(void * dest, const void * src, const size_t n) {
	// If the data is word aligned, then can copy by words
	// Else copy by bytes
	if(dest == src || n == 0 || !dest || !src) {
		return dest;
	}
	
	if(((uintptr_t) dest % sizeof(unsigned int) == 0) &&
	   ((uintptr_t) src  % sizeof(unsigned int) == 0) &&
	   (n % sizeof(unsigned int) == 0)) {
		unsigned int * dest32 = (unsigned int *) dest;
		const unsigned int * src32 = (const unsigned int *) src;
		const size_t n32 = n / sizeof(unsigned int);
		
		for(size_t i = 0; i < n32; i++) {
			dest32[i] = src32[i];
		}
	} else {
		unsigned char * dest8 = (unsigned char *) dest;
		const unsigned char * src8 = (const unsigned char *) src;
		
		for(size_t i = 0; i < n; i++) {
			dest8[i] = src8[i];
		}
	}
	
	return dest;
}

void * memmove(void * dest, const void * src, const size_t n) {
	// If the data is word aligned, then can copy by words
	// Else copy by bytes
	if(dest == src || n == 0 || !dest || !src) {
		return dest;
	}
	
	if(((uintptr_t) dest % sizeof(unsigned int) == 0) &&
	   ((uintptr_t) src  % sizeof(unsigned int) == 0) &&
	   (n % sizeof(unsigned int) == 0)) {
		unsigned int * dest32 = (unsigned int *) dest;
		const unsigned int * src32 = (const unsigned int *) src;
		const size_t n32 = n / sizeof(unsigned int);
		
		if(dest < src) {
			for(size_t i = 0; i < n32; i++) {
				dest32[i] = src32[i];
			}
		} else {
			for(size_t i = n32; i > 0; i--) {
				dest32[i - 1] = src32[i -1];
			}
		}
	} else {
		unsigned char * dest8 = (unsigned char *) dest;
		const unsigned char * src8 = (const unsigned char *) src;
		
		if(dest < src) {
			for(size_t i = 0; i < n; i++) {
				dest8[i] = src8[i];
			}
		} else {
			for(size_t i = n; i > 0; i--) {
				dest8[i - 1] = src8[i - 1];
			}
		}
	}
	
	return dest;
}

void * memset(void * src, const int c, size_t n) {
	unsigned char uc = (unsigned char) c;
	unsigned char * ptr = (unsigned char *) src;
	while(n--) {
		*ptr++ = uc;
	}
	return src;
}

size_t strlen(const char * src) {
	size_t len = 0;
	
	if(!src) {
		return 0;
	}
	while (src[len])
		len++;
	return len;
}

int strcmp(const char * s1, const char * s2) {
	if(s1 == s2 || !s1 || !s2) {
		return 0;
	}
	
	const unsigned char * str1 = (const unsigned char *) s1;
	const unsigned char * str2 = (const unsigned char *) s2;
	
	while(*str1 == *str2) {
		if(*str1 == '\0') {
			return 0;
		}
		str1++;
		str2++;
	}
	return ((*str1 < *str2) ? -1 : 1);
}

/*char * strcpy(const char * dest, const char * src) (
	if(dest == src || n == 0 || !dest || !src) {
		return dest;
	}
	
	if(((uintptr_t) dest % sizeof(unsigned int) == 0) &&
	   ((uintptr_t) src  % sizeof(unsigned int) == 0) &&
	   (n % sizeof(unsigned int) == 0)) {
		unsigned int * dest32 = (unsigned int *) dest;
		const unsigned int * src32 = (const unsigned int *) src;
		const size_t n32 = n / sizeof(unsigned int);
		
		for(size_t i = 0; i < n32; i++) {
			dest32[i] = src32[i];
		}
	} else {
		unsigned char * dest8 = (unsigned char *) dest;
		const unsigned char * src8 = (const unsigned char *) src;
		
		for(size_t i = 0; i < n; i++) {
			dest8[i] = src8[i];
		}
	}
	
	return dest;
}*/
