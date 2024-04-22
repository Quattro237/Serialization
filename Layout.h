#pragma once

#include "Helper.h"

//Enumerates all possible types that can be used in protocol (Type string is a zero-terminated string).
typedef enum Kind {
    INT64,
    UINT64,
    UINT32,
    INT32,
    DOUBLE,
    VARINT,
    BOOL,
    STRING,
    CONSTRUCTED
} Kind;

//Layout is used to decribe layout of type. Can be constructed by user.
typedef struct Layout {
    const struct Layout **fields;
    Kind kind;
    size_t field_q;
} Layout;

//pre-defined layouts for built-in types
extern const Layout INT64_LAYOUT;
extern const Layout UINT64_LAYOUT;
extern const Layout INT32_LAYOUT;
extern const Layout UINT32_LAYOUT;
extern const Layout UINT32_LAYOUT;
extern const Layout DOUBLE_LAYOUT;
extern const Layout VARINT_LAYOUT;
extern const Layout BOOL_LAYOUT;
extern const Layout STRING_LAYOUT;

typedef struct LayoutNode {
    struct LayoutNode* next;
    struct LayoutNode* prev;
} LayoutNode;

//Head of list with allocated layouts
static LayoutNode* CUSTOM_LAYOUTS;

//Clears all custom layouts from memory
void ClearLayouts();
//Creates custom layout and returns pointer to it
const Layout* CreateLayout(size_t field_q, ...);
//Deletes layout by its pointer and returns 0. If layout doesn't exists, returns 1.
int DeleteLayout(const Layout* layout);

//Value is used to fill fields of certain defined layout by values.
typedef struct Value {
    struct Value* children;
    union {
        uint64_t uint_value;
        int64_t int_value;
        char *varint_value;
        char *string_ptr;
        char bool_value;
        double double_value;
    };
} Value;

//Constructs and returns value by using layout and array of pointers to data to fill built-in types.
Value ConstructValue(const Layout *layout, void **values);