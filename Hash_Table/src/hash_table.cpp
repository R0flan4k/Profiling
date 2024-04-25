#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "hash_table.h"
#include "list.h"
#include "my_assert.h"
#include "file_processing.h"
#include "my_strings.h"

const size_t STRING_SIZE = 64;
const size_t VECTOR256I_SIZE = 32;
const char * HIST_DATA_NAME = "hist_data";

static HashTError_t hash_table_calculate_load_factor_and_dispersion(HashTable * hash_table);
static void hash_table_set_function(HashTable * hash_table, HashFunction_t hash_function);
static HashTError_t string_to_vector256i(const char * string, __m256i * vector);
static bool is_in_list(LinkedList * lst, Elem_t word);

HashTError_t hash_table_ctor(HashTable * hash_table, const size_t start_size, HashFunction_t hash_function)
{
    MY_ASSERT(hash_table);

    if (!(hash_table->hash_sets = (LinkedList *) calloc(start_size, sizeof(LinkedList))))
    {
        return HASH_TABLE_ERROR_CANT_CREATE;
    }
    hash_table->size = start_size;
    hash_table->load_factor = 0;
    hash_table->dispersion = 0;

    for (size_t i = 0; i < start_size; i++)
    {
        if (list_ctor(&hash_table->hash_sets[i]))
            return HASH_TABLE_ERROR_CANT_CREATE;
    }
    hash_table_set_function(hash_table, hash_function);

    return 0;
}


HashTError_t hash_table_dtor(HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    for (size_t i = 0; i < hash_table->size; i++)
    {
        list_dtor(&hash_table->hash_sets[i]);
    }

    free(hash_table->hash_sets);
    hash_table->size = 0;
    hash_table->hash_function = NULL;
    hash_table->load_factor = 0;
    hash_table->dispersion = 0;

    return 0;
}


HashTError_t hash_table_recreate(HashTable * hash_table, HashFunction_t hash_function)
{
    MY_ASSERT(hash_table);

    for (size_t i = 0; i < hash_table->size; i++)
    {
        if (list_clear(&hash_table->hash_sets[i]))
            return HASH_TABLE_ERROR_CANT_CLEAR;
    }
    hash_table->load_factor = 0;
    hash_table->dispersion = 0;
    hash_table_set_function(hash_table, hash_function);

    return 0;
}


HashTError_t hash_table_load_hist(const HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    static size_t data_num = 0;
    char data_name[STRING_SIZE] = "";
    sprintf(data_name, "%s%zd.data", HIST_DATA_NAME, data_num);
    data_num++;

    FILE * fp = file_open(data_name, "w");
    if (!fp)
    {
        return HASH_TABLE_ERROR_CANT_OPEN_LOAD_FACTOR_FILE;
    }

    for (size_t i = 0; i < hash_table->size; i++)
    {
        fprintf(fp, "%zd ", i);
    }
    fprintf(fp, "\n");
    for (size_t i = 0; i < hash_table->size; i++)
    {
        fprintf(fp, "%zd ", hash_table->hash_sets[i].size);
    }
    fprintf(fp, "\n%.2f\n%.2f", hash_table->load_factor, hash_table->dispersion);

    fclose(fp);
    
    return 0;
}


static HashTError_t hash_table_calculate_load_factor_and_dispersion(HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    HashTError_t errors = 0;
    if (hash_table->size == 0)
    {
        errors |= HASH_TABLE_ERROR_CALCULATING_EMPTY_HT_LOAD_FACTOR;
    }

    size_t load_sum = 0;
    for (size_t i = 0; i < hash_table->size; i++)
    {
        load_sum += hash_table->hash_sets[i].size;
    }
    hash_table->load_factor =  (float) load_sum / (float) hash_table->size;

    float dispersion_sum = 0;
    for (size_t i = 0; i < hash_table->size; i++)
    {
        float covariation = (float) hash_table->hash_sets[i].size - hash_table->load_factor;
        dispersion_sum += covariation * covariation;
    }
    hash_table->dispersion = dispersion_sum / (float) hash_table->size;

    return errors;
}


HashTError_t get_word_vectors(const char * buffer, __m256i word_vectors[], const size_t words_num)
{
    MY_ASSERT(buffer);

    const char * word_pointer = skip_spaces(buffer);
    for (size_t i = 0; i < words_num; i++)
    {
        string_to_vector256i(word_pointer, word_vectors + i);
        word_pointer = strchr(word_pointer, '\0') + 1;
        word_pointer = skip_spaces(word_pointer);
    }

    return 0;
}


static HashTError_t string_to_vector256i(const char * string, __m256i * vector)
{
    MY_ASSERT(string);
    MY_ASSERT(vector);

    HashTError_t errors = 0;
    size_t word_len = strlen(string);

    if (word_len >= VECTOR256I_SIZE)
    {
        errors |= HASH_TABLE_ERROR_VECTOR_OVERFLOW;
        word_len = VECTOR256I_SIZE - 1;
    }

    memcpy(vector, string, word_len);
    #if 0
        printf("string: %s\nvector: ", string);
        for (size_t i = 0; i < 32; i++)
        {
            printf("%d ", ((char *)vector)[i]);
        }
        printf("\n\n");
    #endif
    
    return errors;
}


HashTError_t hash_table_fill(HashTable * hash_table, Elem_t objects, size_t objects_num)
{
    MY_ASSERT(hash_table);
    MY_ASSERT(objects);
    MY_ASSERT(hash_table->hash_function);

    if (hash_table->hash_function == NULL)
    {
        return HASH_TABLE_ERROR_EMPTY_HASH_FUNCTION;
    }

    for (size_t i = 0; i < objects_num; i++)
    {
        size_t hash_value = hash_table->hash_function(objects + i) % hash_table->size;
        if (!is_in_list(&hash_table->hash_sets[hash_value], objects + i))
        {
            if (list_push_front(&hash_table->hash_sets[hash_value], objects + i))
            {
                return HASH_TABLE_ERROR_CANT_PUSH_OBJECT;
            }
        }
    }
    hash_table_calculate_load_factor_and_dispersion(hash_table);

    return 0;
}


static void hash_table_set_function(HashTable * hash_table, HashFunction_t hash_function)
{
    MY_ASSERT(hash_table);
    MY_ASSERT(hash_function);

    hash_table->hash_function = hash_function;

    return;
}


static bool is_in_list(LinkedList * lst, Elem_t word)
{
    MY_ASSERT(lst);
    MY_ASSERT(word);

    __m256i cmp_res = _mm256_set1_epi8(0);
    int cmp_res_mask = 0;
    for (size_t i = lst->head; i != lst->next[lst->tail]; i = lst->next[i])
    {
        MY_ASSERT(lst->data[i]);
        cmp_res = _mm256_cmpeq_epi8(*word, *(lst->data[i])); 
        cmp_res_mask = _mm256_movemask_epi8(cmp_res);

        if (!(cmp_res_mask + 1))
        {
            return true;
        }
    }

    return false;
}
