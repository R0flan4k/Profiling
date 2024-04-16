#include <stdio.h>
#include <string.h>

#include "hash_table.h"
#include "list.h"
#include "my_assert.h"
#include "file_processing.h"

const size_t STRING_SIZE = 64;
const char * HIST_SCRIPT_NAME = "./python_scripts/hist_script";

HashTError_t hash_table_ctor(HashTable * hash_table, const size_t start_size)
{
    MY_ASSERT(hash_table);

    if (!(hash_table->hash_sets = (LinkedList *) calloc((start_size), sizeof(LinkedList))))
    {
        return HASH_TABLE_ERROR_CANT_CREATE;
    }
    hash_table->size = start_size;
    hash_table->hash_function = NULL;

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

    return 0;
}


HashTError_t hash_table_load_hist(HashTable * hash_table)
{
    MY_ASSERT(hash_table);

    static size_t script_num = 0;
    char script_name[STRING_SIZE] = "";
    sprintf(script_name, "%s%zd.py", HIST_SCRIPT_NAME, script_num);
    script_num++;

    FILE * fp = NULL;
    if (!(fp = file_open(script_name, "w")))
    {
        return HASH_TABLE_ERROR_CANT_OPEN_LOAD_FACTOR_FILE;
    }

    fprintf(fp, "from plotly import graph_objects as go\n\
                 \rx = [ ");
    for (size_t i = 0; i < hash_table->size; i++)
    {
        fprintf(fp, "\"%zd\", ", i);
    }
    fprintf(fp, "]\n\
                 \ry = [ ");
    for (size_t i = 0; i < hash_table->size; i++)
    {
        fprintf(fp, "\"%zd\", ", list_get_size(&hash_table->hash_sets[i]));
    }
    fprintf(fp, "]\n\
                 \rfig = go.Figure()\n\
                 \rfig.add_trace(go.Histogram(histfunc=\"sum\", y=y, x=x, name=\"sum\"))\n\
                 \rfig.show()\n");

    fclose(fp);
    
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