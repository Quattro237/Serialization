#include "Layout.h"
#include <stdarg.h>
#include <string.h>

//pre-defined layouts for built-in types
const Layout INT64_LAYOUT = {.fields = NULL, .kind = INT64,.field_q = 0};
const Layout UINT64_LAYOUT = {.fields = NULL, .kind = UINT64, .field_q = 0};
const Layout INT32_LAYOUT = {.fields = NULL, .kind = INT32, .field_q = 0};
const Layout UINT32_LAYOUT = {.fields = NULL, .kind = UINT32, .field_q = 0};
const Layout DOUBLE_LAYOUT = {.fields = NULL, .kind = DOUBLE, .field_q = 0};
const Layout VARINT_LAYOUT = {.fields = NULL, .kind = VARINT, .field_q = 0};
const Layout BOOL_LAYOUT = {.fields = NULL, .kind = BOOL, .field_q = 0};
const Layout STRING_LAYOUT = {.fields = NULL, .kind = STRING, .field_q = 0};

//Head of list with allocated layouts
static LayoutNode* CUSTOM_LAYOUTS = NULL;

//Pushes layout node into CUSTOM_LAYOUTS
static void PushLayoutNode(LayoutNode* layout_node) {
    if (CUSTOM_LAYOUTS == NULL) {
        CUSTOM_LAYOUTS = layout_node;
        return;
    }
    layout_node->next = CUSTOM_LAYOUTS;
    CUSTOM_LAYOUTS->prev = layout_node;
    CUSTOM_LAYOUTS = layout_node;
}

//Clears all layouts in CUSTOM_LAYOUTS
void ClearLayouts() {
    LayoutNode* curr_node = CUSTOM_LAYOUTS;
    while (curr_node != NULL) {
        LayoutNode* next_node = curr_node->next;
        free(curr_node);
        curr_node = next_node;
    }
}

//Creates layout with field_q fields and Layout* args as fields, and pushes new layout into CUSTOM_LAYOUTS. If error arises returns NULL.
const Layout* CreateLayout(size_t field_q, ...) {
    va_list ap;
    va_start(ap, field_q);

    LayoutNode* new_layout_node = malloc(sizeof(LayoutNode) + sizeof(Layout) + field_q * sizeof(Layout*));
    if (new_layout_node == NULL) {
        return NULL;
    }

    new_layout_node->next = NULL;
    new_layout_node->prev = NULL;
    Layout* new_layout = (Layout*)(new_layout_node + 1);
    new_layout->fields = (const Layout**)(new_layout + 1);
    new_layout->kind = CONSTRUCTED;

    for (size_t i = 0; i < field_q; ++i) {
        const Layout* curr_layout = va_arg(ap, const Layout*);
        new_layout->fields[i] = curr_layout;
    }
    va_end(ap);

    PushLayoutNode(new_layout_node);
    return new_layout;
}

//Deletes layout and returns 0. If layout is not founded returns 1.
int DeleteLayout(const Layout* layout) {
    LayoutNode* curr_node = CUSTOM_LAYOUTS;
    while (curr_node != NULL) {
        Layout* curr_layout = (Layout*)(curr_node + 1);
        if (curr_layout == layout) {
            LayoutNode* prev_layout_node = curr_node->prev;
            LayoutNode* next_layout_node = curr_node->next;
            if (prev_layout_node != NULL) {
                prev_layout_node->next = next_layout_node;
            }
            if (next_layout_node != NULL) {
                next_layout_node->prev = prev_layout_node;
            }
            free(curr_node);
            return 0;
        }
        curr_node = curr_node->next;
    }
    return 1;
}

//Constructs and returns value by using layout and array of pointers to data to fill built-in types.
Value ConstructValue(const Layout *layout, void **values) {
    err_code = 0;
    Value constructed_value = {NULL, 0};
    if (layout->kind != CONSTRUCTED) {
        if (layout->kind == INT64) {
            constructed_value.int_value = *(int64_t *)values;
        } else if (layout->kind == UINT64) {
            constructed_value.uint_value = *(uint64_t *)values;
        } else if (layout->kind == INT32) {
            constructed_value.int_value = *(int32_t *)values;
        } else if (layout->kind == UINT32) {
            constructed_value.uint_value = *(uint32_t *)values;
        } else if (layout->kind == VARINT) {
            constructed_value.varint_value = *(char **) values;
        } else if (layout->kind == DOUBLE) {
            constructed_value.double_value = *(double *)values;
        } else if (layout->kind == BOOL) {
            constructed_value.bool_value = *(char *)values;
        } else if (layout->kind == STRING) {
            char *str_ptr = *(char **)values;
            char *str_copy = calloc(strlen(str_ptr), sizeof(*str_copy));
            strcpy(str_copy, str_ptr);
            constructed_value.string_ptr = str_copy;
        } else {
            err_code = 1;
            perror("Unknown type in layout\n");
        }
    } else {
        constructed_value.children = calloc(layout->field_q, sizeof(*constructed_value.children));
        for (size_t i = 0; i < layout->field_q; ++i) {
            constructed_value.children[i] = ConstructValue(layout->fields[i], values[i]);
        }
    }
    return constructed_value;
}