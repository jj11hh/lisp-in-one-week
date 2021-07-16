#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOC(objtype) ((objtype*)(malloc(sizeof(objtype))))

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


void lisp_print_value(void *obj);
struct lisp_int *make_int(int number);
struct lisp_str *make_str(char *cstring);
struct lisp_cons *make_cons(void *car, void *cdr);
void *car(struct lisp_cons *cons);
void *cdr(struct lisp_cons *cons);

struct lisp_cons *lisp_parse(const char * source);

struct lisp_cons *globals = NULL;

void add_to_globals(char *var_name, void *var_value){
    struct lisp_cons *item = make_cons(make_str(var_name), var_value);
    globals = make_cons(item, globals);
}

void *lookup_variable(char *var_name) {
    struct lisp_cons *cur = globals;
    for (;cur;cur = cur->cdr) {
        if (strcmp(((struct lisp_str*)((struct lisp_cons*)cur->car)->car)->value, var_name) == 0) {
            return ((struct lisp_cons*)cur->car)->cdr;
        }
    }

    return NULL;
}

void lisp_print_value(void *obj) {
    if (!obj) {
        printf("null");
        return;
    }

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


struct lisp_int *make_int(int number) {
    // make a new int object
    struct lisp_int *intobj = ALLOC(struct lisp_int);
    intobj->objhead.objtype = INT;
    intobj->value = number;

    return intobj;
}

struct lisp_str *make_str(char *cstring) {
    // make a new string object
    struct lisp_str *strobj = ALLOC(struct lisp_str);
    strobj->objhead.objtype = STR;
    strobj->value = cstring;

    return strobj;
}

struct lisp_cons *make_cons(void *car, void *cdr) {
    // make a cons
    struct lisp_cons *consobj = ALLOC(struct lisp_cons);
    consobj->objhead.objtype = CONS;
    consobj->car = car;
    consobj->cdr = cdr;
    
    return consobj;
}

void *car(struct lisp_cons *cons) {
    return cons->car;
}

void *cdr(struct lisp_cons *cons) {
    return cons->cdr;
}

struct parse_ctx {
    const char *current;
    int line_no; 
};

#define peek(ctx) (*((ctx)->current))
#define take(ctx) (*((ctx)->current++))

static void* parse_expr(struct parse_ctx *ctx);

static void parse_error(struct parse_ctx *ctx) {
    fprintf(stderr, "parse_error at line number: %d", ctx->line_no);
    exit(-1);
}

static int char_in(char ch, char * haystack){
    for (; *haystack; haystack++) {
        if (ch == *haystack) return 1;
    }
    return 0;
}

static void skip_blank(struct parse_ctx *ctx) {
    while (1) {
        char c = peek(ctx);
        if (c == ' ' || c == '\t') {
            take(ctx);
            continue;
        }
        if (c == '\n') {
            ctx->line_no ++;
            take(ctx);
            continue;
        }

        break;
    }
}

static int parse_int(struct parse_ctx *ctx){
    int result = 0;
    int sign = 0;
    if (! char_in(peek(ctx), "+-0123456789")) {
        parse_error(ctx);
    }

    char c = take(ctx);
    if (c == '-') sign = 1;
    // is a digit
    else if (c != '+') {
        result = c - '0';
    }

    while (char_in(peek(ctx), "_0123456789")) {
        c = take(ctx);
        if (c == '_') continue;

        result *= 10;
        result += c - '0';
    }

    result = sign ? -result : result;

    return result;
}

static char read_escape(struct parse_ctx *ctx) {
    char c = take(ctx);
    switch (c) {
        case '\0':
        parse_error(ctx);
        break;

        case '"':
        return '"';

        case 'n':
        return '\n';

        case 't':
        return '\t';

        case 'b':
        return '\b';
    }
}

static char * parse_str(struct parse_ctx *ctx){
    static char buf[4096] = {0};

    int len = 0;

    if (take(ctx) != '"') {
        parse_error(ctx);
    }

    for(;;){
        char c = take(ctx);
        switch (c) {
            case '\0':
            // unexcepted end of string
            parse_error(ctx);
            break;

            case '\\':
            // escape character
            buf[len++] = read_escape(ctx);
            break;

            case '"':
            goto OUT;

            default:
            buf[len++] = c;
        }
    }
    OUT:

    // terminate c string
    buf[len] = 0;
    char * new_str = malloc(len+1);
    strcpy(new_str, buf);

    return new_str;
}

static struct lisp_cons* parse_list(struct parse_ctx *ctx) {
    if (take(ctx) != '(')
        parse_error(ctx);

    struct lisp_cons* list = NULL;

    void *expr = NULL;
    while (expr = parse_expr(ctx)) {
        skip_blank(ctx);

        list = make_cons(expr, list);
    }

    if (take(ctx) != ')')
        parse_error(ctx);

    // null for empty list
    return list;
}

static void* parse_expr(struct parse_ctx *ctx) {
    char c = peek(ctx);
    if (c == '"')
        return make_str(parse_str(ctx));

    if (c == '+' || c == '-' || ('0' <= c && c <= '9')) {
        return make_int(parse_int(ctx));
    }

    if (c == '(')
        return parse_list(ctx);

    return NULL;
}

struct lisp_cons* lisp_parse(const char * source) {
    
}

int main(int argc, char **argv) {
    struct parse_ctx ctx;
    ctx.current = "(1 2 3 4 \"fuck\" (114514 ()) 1919)";
    ctx.line_no = 0;

    // make a new int object
    add_to_globals("varint", make_int(114514));
    
    // make a new string object
    add_to_globals("varstr", make_str("hello world"));

    // make a cons
    add_to_globals("varcons", make_cons(
        lookup_variable("varint"),
        lookup_variable("varstr")
    ));

    // make a list
    add_to_globals("list", parse_expr(&ctx));

    lisp_print_value(lookup_variable("varint"));
    putchar('\n');

    lisp_print_value(lookup_variable("varstr"));
    putchar('\n');

    lisp_print_value(lookup_variable("varcons"));
    putchar('\n');

    lisp_print_value(lookup_variable("list"));
    putchar('\n');

    lisp_print_value(globals);
    putchar('\n');
}