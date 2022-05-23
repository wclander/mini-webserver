/* 
    Contains logging functions that print to the file specified by set_log_file() 
    or standard output if no file is specified
*/

/*
Prints the given message as if printf was used, followed by a newline
*/
void debug_log(char *message, ...);

/*
Sets the logging output file to the given FILE * or stderr if FILE * is NULL
*/
void set_log_file(FILE *file_ptr);

/*
Sets the logging output file to the file at the given address (absolute or relative)
    or stderr if filename is blank or invalid
*/
void open_log_file(char *filename);

/*
Closes the current log output file if it is not stderr
*/
void close_log_file();
