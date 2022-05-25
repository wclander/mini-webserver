#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

FILE *logging_print_file = NULL;

void debug_log(char *message, ...) {
    va_list ap;
    va_start(ap, message);
    vfprintf(logging_print_file, message, ap);
    va_end(ap);
    fprintf(logging_print_file,  "\n");
}

void set_log_file(FILE *file_ptr){
    if (file_ptr == NULL) {
        logging_print_file = stderr; 
    } else {
        logging_print_file = file_ptr;
    }
}

void open_log_file(char *filename) {
    // check if NULL or empty
    if (filename == NULL || *filename == '\0') {
        set_log_file(NULL);
    } else {
        // since fopen returns NULL if filename is invalid or cannot be opened
        set_log_file(fopen(filename, "a+"));
    }
}

void close_log_file() {
    if (logging_print_file != stderr)
        fclose(logging_print_file);
}
