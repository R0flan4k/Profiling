#ifndef FLAGS_H
    #define FLAGS_H

    #include "cmd_input.h"

    extern CmdLineArg HASH_TABLE_SOURCE_FILE;
    extern const char * HASH_TABLE_SOURCE_FILE_NAME;

    extern char * * CMD_INPUT;
    extern CmdLineArg * FLAGS[];
    extern size_t FLAGS_ARRAY_SIZE;

    void show_error_message(const char * program_name);
    
    void set_hash_table_source_file(void);

#endif // FLAGS_H