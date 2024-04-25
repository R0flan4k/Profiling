#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "my_strings.h"
#include "my_assert.h"


void show_text(const char * text5, const size_t size)
{
    size_t i = 0;

    while (i < size)
    {
        fputc(text5[i], stdout);

        i++;
    }
}


size_t get_strings_num(char * buffer)
{
    size_t strings_num = 0;
    char * string_pointer = buffer;

    while ((string_pointer = strchr(string_pointer, '\n')) != NULL)
    {
        *string_pointer = '\0';
        string_pointer++;
        strings_num++;
    }

    return strings_num + 1;
}


size_t get_words_num(char * buffer)
{
    MY_ASSERT(buffer);

    size_t words_num = 0;
    char * buffer_ptr = skip_spaces(buffer);

    while ((buffer_ptr = get_next_word(buffer_ptr)) != NULL)
    {
        *buffer_ptr = '\0';
        buffer_ptr++;
        words_num++;
        buffer_ptr = skip_spaces(buffer_ptr);
    }

    return words_num;
}


void get_pointers(const char * buffer, const char * pointers[], const size_t strings_num)
{
    size_t i = 0;
    char const * string_pointer = buffer;

    while (i < strings_num)
    {
        pointers[i] = string_pointer;

        string_pointer = strchr(string_pointer, '\0') + 1;

        i++;
    }
}


void switch_strings(char * * string1, char * * string2)
{
    char * tmp_string = *string1;

    *string1 = *string2;
    *string2 = tmp_string;
}


void show_strings(const char * const * strings, const size_t strings_num)
{
    for (size_t i = 0; i < strings_num; i++)
    {
        printf("%s\n", strings[i]);
    }
}


char * skip_spaces(const char * buffer)
{
    MY_ASSERT(buffer);

    const char * buffer_ptr = buffer;

    while (isspace(*buffer_ptr))
        buffer_ptr++;

    return const_cast <char *> (buffer_ptr);
}


char * get_next_word(char * buffer)
{
    MY_ASSERT(buffer);

    char * buffer_ptr = buffer;

    while(!isspace(*buffer_ptr))
    {
        if (*buffer_ptr == '\0')
            return NULL;
        buffer_ptr++;
    }

    return buffer_ptr;
}