#include <stdio.h>

struct lisp_objhead {
    enum {
        INT = 1,
        STRING = 5,
    } objType;
};

struct lisp_ivalue {
    struct lisp_objhead objhead;
    int value;
};

struct lisp_str {
    struct lisp_objhead objhead;
    char *value;
};

void lisp_print_value(void *obj) {
    switch (((struct lisp_objhead*)obj)->objType) {
        case INT:
        printf("<int %d>", ((struct lisp_ivalue*)obj)->value);
        break;

        case STRING:
        printf("<string %s>", ((struct lisp_str*)obj)->value);
        break;
    }
}

int main(int argc, char **argv) {
    // make a new int object
    struct lisp_ivalue intobj;
    intobj.objhead.objType = INT;
    intobj.value = 114514;
    
    // make a new string object
    struct lisp_str strobj;
    strobj.objhead.objType = STRING;
    strobj.value = "hello, world";

    lisp_print_value(&intobj);
    putchar('\n');

    lisp_print_value(&strobj);
    putchar('\n');
}