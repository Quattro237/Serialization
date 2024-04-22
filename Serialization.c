#include "Serialization.h"

//Finds size of serialized data in bytes
static size_t FindSerializedSize(const Layout* layout, Value* value) {
    size_t byte_size = 0;
    if (layout->kind == INT64 || layout->kind == UINT64) {
        byte_size += 8;
    } else if (layout->kind == INT32 || layout->kind == UINT32) {
        byte_size += 4;
    } else if (layout->kind == VARINT) {
        char* curr_byte = value->varint_value;
        while (*curr_byte >> 7) {
            ++curr_byte;
        }
        byte_size += (curr_byte - value->varint_value) + 1;
    } else if (layout->kind == CONSTRUCTED) {
        for (size_t i = 0; i < layout->field_q; ++i) {
            byte_size += FindSerializedSize(layout->fields[i], &value->children[i]);
        }
    }
    return byte_size;
}

static int SerializeINT64(char* buffer, size_t* iter, int64_t inp) {
    char* inp_ptr = (char *)(&inp);
    for (size_t i = 0; i < 8; ++i) {
        buffer[*iter] = inp_ptr[i];
        *iter += 1;
    }
    return 0;
}

static int SerializeUINT64(char* buffer, size_t* iter, uint64_t inp) {
    char* inp_ptr = (char *)(&inp);
    for (size_t i = 0; i < 8; ++i) {
        buffer[*iter] = inp_ptr[i];
        *iter += 1;
    }
    return 0;
}

static int SerializeINT32(char* buffer, size_t* iter, uint64_t inp) {
    char* inp_ptr = (char *)(&inp);
    for (size_t i = 0; i < 4; ++i) {
        buffer[*iter] = inp_ptr[i];
        *iter += 1;
    }
    return 0;
}

static int SeralizeUINT32(char* buffer, size_t* iter, uint64_t inp) {
    char* inp_ptr = (char *)(&inp);
    for (size_t i = 0; i < 4; ++i) {
        buffer[*iter] = inp_ptr[i];
        *iter += 1;
    }
    return 0;
}

static int SerializeVARINT(char* buffer, size_t* iter, char* varint) {
    while (*varint >> 7) {
        buffer[*iter] = *varint;
        *iter += 1;
        ++varint;
    }
    buffer[*iter] = *varint;
    *iter += 1;

    return 0;
}

static int RecSerialize(char* buffer, size_t* iter, const Layout* layout, Value* value) {
    int exit_code = 0;
    if (layout->kind == INT64) {
        return SerializeINT64(buffer, iter, (int64_t)(value->int_value));
    } else if (layout->kind == UINT64) {
        return SerializeUINT64(buffer, iter, (uint64_t)(value->uint_value));
    } else if (layout->kind == INT32) {
        return SerializeINT32(buffer, iter, (int32_t)(value->int_value));
    } else if (layout->kind == UINT32) {
        return SeralizeUINT32(buffer, iter, (uint32_t)(value->uint_value));
    } else if (layout->kind == VARINT) {
        return SerializeVARINT(buffer, iter, value->varint_value);
    } else if (layout->kind == CONSTRUCTED) {
        for (size_t i = 0; i < layout->field_q; ++i) {
            int exit_code = RecSerialize(buffer, iter, layout->fields[i], &value->children[i]);
            if (exit_code != 0) {
                return exit_code;
            }
        }
        return 0;
    }
    return 1;
}

//Serializes values by layout. Returns bytes of serialized data.
char* Serialize(const Layout* layout, Value value) {
    char* buffer = (char *)(calloc(FindSerializedSize(layout, &value), sizeof(char)));
    size_t iter = 0;
    int exit_code = RecSerialize(buffer, &iter, layout, &value);
    if (exit_code == 0) {
        return buffer;
    } else {
        return NULL;
    }
}


