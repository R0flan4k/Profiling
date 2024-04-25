#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

#include "my_strings.h"
#include "my_assert.h"
#include "hash_table.h"
#include "hash_functions.h"
#include "file_processing.h"
#include "flags.h"


int main(int argc, char * argv[])
{
    if (check_cmd_input(argc, argv) == false)
        return 1;

    FILE * fp = NULL;
    if (!(fp = file_open(HASH_TABLE_SOURCE_FILE_NAME, "r")))
        return 1;

    long file_size = get_file_size(fp);
    long buffer_size = file_size + 1;

    char * buffer = NULL;
    if (!(buffer = (char *) calloc(buffer_size, sizeof(char))))
    {
        fclose(fp);
        return 1;
    }
    if (!(buffer = read_file(buffer, buffer_size, fp)))
    {
        fclose(fp);
        free(buffer);
        return 1;
    }
    fclose(fp);
    buffer[buffer_size - 1] = '\0';

    size_t words_num = get_words_num(buffer);
    __m256i * word_vectors = NULL;
    if (!(word_vectors = (__m256i *) calloc(words_num, sizeof(__m256i))))
    {
        free(buffer);
        return 1;
    }
    get_word_vectors(buffer, word_vectors, words_num);

    HashTable hash_table = {};
    const size_t hash_table_size = 100;
    HashTError_t ht_errors = 0;
    HashFunction_t test_hash_functions[] = {hash_table_function_const, hash_table_function_fl,   
                                            hash_table_function_len,   hash_table_function_sum};
    size_t test_hash_functions_num = sizeof(test_hash_functions) / sizeof(test_hash_functions[0]);

    for (size_t i = 0; i < test_hash_functions_num; i++)
    {
        ht_errors |= hash_table_ctor(&hash_table, hash_table_size, test_hash_functions[i]);
        ht_errors |= hash_table_fill(&hash_table, word_vectors, words_num);
        ht_errors |= hash_table_load_hist(&hash_table);
        ht_errors |= hash_table_dtor(&hash_table);
    }
    
    free(buffer);
    free(word_vectors);
    return ht_errors;
}
