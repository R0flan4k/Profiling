#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "hash_table.h"
#include "list.h"
#include "my_assert.h"
#include "file_processing.h"

const size_t STRING_SIZE = 64;
const char * HIST_DATA_NAME = "./python_scripts/hist_data";

static HashTError_t hash_table_calculate_load_factor_and_dispersion(HashTable * hash_table);

HashTError_t hash_table_ctor(HashTable * hash_table, const size_t start_size)
{
    MY_ASSERT(hash_table);

    if (!(hash_table->hash_sets = (LinkedList *) calloc((start_size), sizeof(LinkedList))))
    {
        return HASH_TABLE_ERROR_CANT_CREATE;
    }
    hash_table->size = start_size;
    hash_table->hash_function = NULL;
    hash_table->load_factor = 0;
    hash_table->dispersion = 0;

    for (size_t i = 0; i < start_size; i++)
    {
        if (list_ctor(&hash_table->hash_sets[i]))
            return HASH_TABLE_ERROR_CANT_CREATE;
    }

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


HashTError_t hash_table_clear(HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    for (size_t i = 0; i < hash_table->size; i++)
    {
        if (list_clear(&hash_table->hash_sets[i]))
            return HASH_TABLE_ERROR_CANT_CLEAR;
    }
    hash_table->load_factor = 0;
    hash_table->dispersion = 0;

    return 0;
}


HashTError_t hash_table_load_hist(HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    static size_t data_num = 0;
    char data_name[STRING_SIZE] = "";
    sprintf(data_name, "%s%zd.data", HIST_DATA_NAME, data_num);
    data_num++;

    FILE * fp = NULL;
    if (!(fp = file_open(data_name, "w")))
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

    size_t load_sum = 0;
    for (size_t i = 0; i < hash_table->size; i++)
    {
        load_sum += hash_table->hash_sets[i].size;
    }
    hash_table->load_factor =  (float) load_sum / (float) hash_table->size;

    float dispersion_sum = 0;
    for (size_t i = 0; i < hash_table->size; i++)
    {
        dispersion_sum += powf((float) hash_table->hash_sets[i].size - hash_table->load_factor, 2);
    }
    hash_table->dispersion = dispersion_sum / (float) hash_table->size;

    return 0;
}


HashTError_t hash_table_fill(HashTable * hash_table, Elem_t * objects, size_t objects_num)
{
    MY_ASSERT(hash_table);
    MY_ASSERT(objects);

    if (hash_table->hash_function == NULL)
    {
        return HASH_TABLE_ERROR_EMPTY_HASH_FUNCTION;
    }

    for (size_t i = 0; i < objects_num; i++)
    {
        size_t hash_value = hash_table->hash_function(objects[i]) % hash_table->size;
        if (!is_in_list(&hash_table->hash_sets[hash_value], objects[i]))
        {
            if (list_push_front(&hash_table->hash_sets[hash_value], objects[i]))
            {
                return HASH_TABLE_ERROR_CANT_PUSH_OBJECT;
            }
        }
    }
    hash_table_calculate_load_factor_and_dispersion(hash_table);

    return 0;
}


HashTError_t hash_table_set_function(HashTable * hash_table, size_t (*hash_function)(Elem_t object))
{
    MY_ASSERT(hash_table);
    MY_ASSERT(hash_function);

    hash_table->hash_function = hash_function;

    return 0;
}


size_t hash_table_function_fl(Elem_t object)
{
    return (size_t) *object;
}


size_t hash_table_function_len(Elem_t object)
{
    return strlen(object);
}


size_t hash_table_function_sum(Elem_t object)
{
    size_t sum = 0;

    while (*object != '\0')
    {
        sum += *object;
        object++;
    }

    return sum;
}


size_t hash_table_function_const(Elem_t)
{
    return 0;
}