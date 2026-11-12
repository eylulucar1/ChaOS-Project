#include <common/string.h>

void* memcpy(void* dest, const void* src, uint32_t count) {
    uint32_t* d32 = (uint32_t*)dest;
    const uint32_t* s32 = (const uint32_t*)src;
    uint32_t words = count / 4;

    for (uint32_t i = 0; i < words; i++) {
        d32[i] = s32[i];
    }

    uint8_t* d8 = (uint8_t*)dest;
    const uint8_t* s8 = (const uint8_t*)src;
    for (uint32_t i = words * 4; i < count; i++) {
        d8[i] = s8[i];
    }

    return dest;
}

void* memset(void* dest, uint8_t val, uint32_t count) {
    uint8_t* temp = (uint8_t*)dest;
    for (uint32_t i = 0; i < count; i++) {
        temp[i] = val;
    }
    return dest;
}

uint32_t* memset32(uint32_t* dest, uint32_t val, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        dest[i] = val;
    }
    return dest;
}

uint32_t strlen(const char* str) {
    if (!str) return 0;
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int32_t strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

int32_t strncmp(const char* s1, const char* s2, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return (uint8_t)s1[i] - (uint8_t)s2[i];
        }
    }
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* saved = dest;
    while ((*dest++ = *src++) != '\0');
    return saved;
}

char* strcat(char* dest, const char* src) {
    char* ptr = dest + strlen(dest);
    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

void int_to_str(int32_t num, char* buffer) {
    int32_t i = 0;
    bool isNegative = false;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int32_t rem = num % 10;
        buffer[i++] = rem + '0';
        num = num / 10;
    }

    if (isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    int32_t start = 0;
    int32_t end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

void hex_to_str(uint32_t num, char* buffer) {
    const char* hexDigits = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buffer[9 - i] = hexDigits[num & 0xF];
        num >>= 4;
    }
    buffer[10] = '\0';
}