#ifndef COMMON_STRING_H
#define COMMON_STRING_H

#include <common/types.h>

void* memcpy(void* dest, const void* src, uint32_t count);
void* memset(void* dest, uint8_t val, uint32_t count);
uint32_t* memset32(uint32_t* dest, uint32_t val, uint32_t count);

uint32_t strlen(const char* str);
int32_t strcmp(const char* s1, const char* s2);
int32_t strncmp(const char* s1, const char* s2, uint32_t n);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);

void int_to_str(int32_t num, char* buffer);
void hex_to_str(uint32_t num, char* buffer);

#endif