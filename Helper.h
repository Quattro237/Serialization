#pragma once

#include <stdlib.h>
#include <stdio.h>

//Represents whether function ended normally or not.
static int err_code;

//Converts varint by its pointer to uint32_t, saves in res by its pointer, returns 0. If error has occurred, returns 1.
int ConvertVARINTtoUINT32(char* varint, uint32_t* res);
//Converts varint by its pointer to uint64_t, saves in res by its pointer, returns 0. If error has occurred, returns 1.
int ConvertVARINTtoUINT64(char* varint, uint64_t* res);
//Converts uint(uint32_t is elevated to uint64_t) to varint. Pass NULL or pointer to char*, where varint is stored. Old varint will be deleted.
int ConvertUINTtoVARINT(char** varint, uint64_t inp);

