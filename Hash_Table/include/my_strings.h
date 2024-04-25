#ifndef STRINGS_H
    #define STRINGS_H

    #include <string.h>
    #include <ctype.h>

    char * skip_spaces(const char * buffer);
    char * get_next_word(char * buffer);
    void show_text(const char * text5, const size_t size);
    size_t get_strings_num(char * buffer);
    size_t get_words_num(char * buffer);
    void get_pointers(const char * buffer, const char * pointers[], const size_t strings_num);
    void switch_strings(char * * string1, char * * string2);
    void show_strings(const char * const * strings, const size_t strings_num);

#endif // STRINGS_H