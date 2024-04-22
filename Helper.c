#include "Helper.h"

static int err_code = 0;

int ConvertVARINTtoUINT32(char* varint, uint32_t* res) {
    *res = 0;

    char* curr_byte_iter = varint;
    while (*curr_byte_iter >> 7) {
        ++curr_byte_iter;
    }
    size_t varint_size = curr_byte_iter - varint;
    size_t total_bits = varint_size * 7;
    if (*curr_byte_iter != 0) {
        total_bits += 8 - (__builtin_clz(*curr_byte_iter) - 24);
    }
    if (total_bits > 32) {
        return 1;
    }

    size_t curr_shift = 0;
    for (size_t i = 0; i < varint_size; ++i) {
        char curr_byte = varint[i] << 1;
        *res += ((uint32_t)(curr_byte >> 1)) << (curr_shift);
        curr_shift += 7;
    }
    *res += (uint32_t)*curr_byte_iter << curr_shift;
    return 0;
}

int ConvertVARINTtoUINT64(char* varint, uint64_t* res) {
    *res = 0;

    char* curr_byte_iter = varint;
    while (*curr_byte_iter >> 7) {
        ++curr_byte_iter;
    }
    size_t varint_size = curr_byte_iter - varint;
    size_t total_bits = varint_size * 7;
    if (*curr_byte_iter != 0) {
        total_bits += 8 - (__builtin_clz(*curr_byte_iter) - 24);
    }
    if (total_bits > 64) {
        return 1;
    }

    size_t curr_shift = 0;
    for (size_t i = 0; i < varint_size; ++i) {
        char curr_byte = varint[i] << 1;
        *res += ((uint64_t)(curr_byte >> 1)) << (curr_shift);
        curr_shift += 7;
    }
    *res += (uint64_t)*curr_byte_iter << curr_shift;
    return 0;
}

int ConvertUINTtoVARINT(char** varint, uint64_t inp) {
    char* old_varint = *varint;

    if (inp == 0) {
        *varint = (char*)calloc(1, sizeof(*varint));
        if (*varint == NULL) {
            *varint = old_varint;
            return 1;
        }
        free(old_varint);
        (*varint)[0] = 0;
        return 0;
    }
    size_t val_bits = 64 - __builtin_clz(inp);
    size_t one_bytes = val_bits / 7;
    if (val_bits % 7 == 0) {
        --one_bytes;
    }

    *varint = (char*)calloc(one_bytes + 1, sizeof(**varint));
    if (*varint == NULL) {
        *varint = old_varint;
        return 1;
    }
    free(old_varint);

    for (size_t i = 0; i < one_bytes; ++i) {
        char curr_byte = inp;
        curr_byte = ((~(curr_byte >> 7)) << 7) ^ curr_byte;
        (*varint)[i] = curr_byte;
        inp = inp >> 7;
    }
    (*varint)[one_bytes] = inp;
    return 0;
}