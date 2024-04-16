#include <stdio.h>
#include <string.h>

#include "cmd_input.h"
#include "my_assert.h"
#include "flags.h"

const char * HASH_TABLE_SOURCE_FILE_NAME = NULL;
char * * CMD_INPUT = NULL;

CmdLineArg HASH_TABLE_SOURCE_FILE = {
    .name =          "--src",
    .num_of_param =  1,
    .flag_function = set_hash_table_source_file,
    .argc_number =   0,
    .help =          "--src *src file_name*",
    .is_mandatory = false,
};

CmdLineArg * FLAGS[] = {&HASH_TABLE_SOURCE_FILE};
size_t FLAGS_ARRAY_SIZE = sizeof(FLAGS) / sizeof(FLAGS[0]);


void show_error_message(const char * program_name)
{
    printf("Error. Please, use %s %s\n", program_name, HASH_TABLE_SOURCE_FILE.help);
}

void set_hash_table_source_file(void)
{
    HASH_TABLE_SOURCE_FILE_NAME = CMD_INPUT[HASH_TABLE_SOURCE_FILE.argc_number + 1];
}