#include <stdio.h>
#include <stdlib.h>

struct lisp_objhead {
    enum {
        INT = 1,
        STR = 2,
        CONS = 3,
    } objtype;
};

struct lisp_int {
    struct lisp_objhead objhead;
    int value;
};

struct lisp_str {
    struct lisp_objhead objhead;
    char *value;
};

struct lisp_cons {
    struct lisp_objhead objhead;
    void *car;
    void *cdr;
};

void lisp_print_value(void *obj) {
    switch (((struct lisp_objhead*)obj)->objtype) {
        case INT:
        printf("<int %d>", ((struct lisp_int*)obj)->value);
        break;

        case STR:
        printf("<string %s>", ((struct lisp_str*)obj)->value);
        break;

        case CONS:
        printf("<cons ");
        lisp_print_value(((struct lisp_cons*)obj)->car);
        printf(", ");
        lisp_print_value(((struct lisp_cons*)obj)->cdr);
        printf(">");
        break;

        default:
        printf("<unknown-type object>");
    }
}

#define ALLOC(objtype) ((objtype*)(malloc(sizeof(objtype))))

int main(int argc, char **argv) {
    // make a new int object
    struct lisp_int *intobj = ALLOC(struct lisp_int);
    intobj->objhead.objtype = INT;
    intobj->value = 114514;
    
    // make a new string object
    struct lisp_str *strobj = ALLOC(struct lisp_str);
    strobj->objhead.objtype = STR;
    strobj->value = "hello, world";

    // make a cons
    struct lisp_cons *consobj = ALLOC(struct lisp_cons);
    consobj->objhead.objtype = CONS;
    consobj->car = intobj;
    consobj->cdr = strobj;

    lisp_print_value(intobj);
    putchar('\n');

    lisp_print_value(strobj);
    putchar('\n');

    lisp_print_value(consobj);
    putchar('\n');

    free(consobj); free(strobj); free(intobj);
}