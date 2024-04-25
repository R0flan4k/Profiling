#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "list.h"
#include "my_assert.h"
#include "file_processing.h"

const char * LIST_DUMP_FILE_NAME = "./graphviz/list_dump.dot";

const size_t START_CAPACITY = 32;
const size_t DUMMY_NODE_ID = 0;
const size_t TRASH_VALUE = 0xAB1BA5;
const size_t OUTPUT_BUFFER_SIZE = 512;
const Elem_t DATA_TRASH_VALUE = NULL;
const char * BLUE_COLOR = "#004bc4";
const char * GREEN_COLOR = "#0f9900";

static LstError_t set_list_head(LinkedList * lst, size_t new_head_id);
static LstError_t set_list_tail(LinkedList * lst, size_t new_tail_id);
static LstError_t list_reference_arrays_vtor(LinkedList * lst);
static void * real_calloc(void * ptr, size_t old_size, size_t new_size);


LstError_t list_ctor(LinkedList * lst)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (!list_vtor(lst))            // check if data[], next[] or prev[] are not NULL.
    {
        errors |= LIST_ERROR_ALREADY_CONSTRUCTED;
        return errors;
    }

    if (!(lst->data = (Elem_t *) calloc(START_CAPACITY, sizeof(Elem_t))) ||
        !(lst->next = (size_t *) calloc(START_CAPACITY, sizeof(size_t))) ||
        !(lst->prev = (int *) calloc(START_CAPACITY, sizeof(size_t))))
    {
        free(lst->data);
        free(lst->prev);
        free(lst->next);

        errors |= LIST_ERROR_CANT_ALLOCATE_MEMORY;
        return errors;
    }
    lst->size = 0;
    lst->capacity = START_CAPACITY;

    set_list_head(lst, DUMMY_NODE_ID);
    set_list_tail(lst, DUMMY_NODE_ID);

    lst->free = 1;

    for (size_t i = 1; i < START_CAPACITY; i++)
    {
        lst->data[i] = DATA_TRASH_VALUE;
        lst->next[i] = i % (START_CAPACITY - 1) + 1;
        lst->prev[i] = -1;
    }

    return errors;
}


LstError_t list_dtor(LinkedList * lst)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (lst->data == NULL && lst->next == NULL &&
        lst->prev == NULL && lst->capacity == TRASH_VALUE &&
        lst->head == TRASH_VALUE   && lst->tail == TRASH_VALUE &&
        lst->free == TRASH_VALUE)
    {
        errors |= LIST_ERROR_ALREADY_DESTRUCTED;
        return errors;
    }

    if (errors = list_vtor(lst))
    {
        return errors;
    }

    free(lst->data);
    free(lst->next);
    free(lst->prev);

    lst->data = (Elem_t *) NULL;
    lst->next = (size_t *) NULL;
    lst->prev = (int *) NULL;

    lst->size     = TRASH_VALUE;
    lst->capacity = TRASH_VALUE;
    lst->head     = TRASH_VALUE;
    lst->tail     = TRASH_VALUE;
    lst->free     = TRASH_VALUE;

    return errors;
}


LstError_t list_vtor(LinkedList * lst)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    // if (lst->size != list_get_size(lst))
    //     errors |= LIST_ERROR_INVALID_SIZE;
    if (lst->capacity == 0)
        errors |= LIST_ERROR_INVALID_CAPACITY;
    if (lst->head >= lst->capacity)
        errors |= LIST_ERROR_INVALID_HEAD;
    if (lst->tail >= lst->capacity)
        errors |= LIST_ERROR_INVALID_TAIL;
    if (lst->free < -1 && lst->free >= (int) lst->capacity)
        errors |= LIST_ERROR_INVALID_FREE;

    if (!lst->data)
        errors |= LIST_ERROR_INVALID_DATA;

    LstError_t reference_arrays_errors = 0;
    if (reference_arrays_errors = list_reference_arrays_vtor(lst))
        errors |= reference_arrays_errors;

    return errors;
}


void list_dump_internal(LinkedList * lst,
                        const char * lst_name, const char * func,
                        const int line, const char * file)
{
    MY_ASSERT(lst);
    MY_ASSERT(lst->data);
    MY_ASSERT(lst->next);
    MY_ASSERT(lst->prev);
    MY_ASSERT(lst_name);
    MY_ASSERT(func);
    MY_ASSERT(file);

    FILE * fp = NULL;
    if (!(fp = file_open(LIST_DUMP_FILE_NAME, "wb")))
    {
        return;
    }
///
    fprintf(fp, "digraph G\n"
                "{\n"
                "\tgraph [dpi = 150]\n"
                "\tranksep = 0.5;\n"
                "\tbgcolor = \"#ffffff\"\n"
                "\tsplines = ortho\n"
                "\tedge[minlen = 3];\n"
                "\tnode[shape = record, style = \"rounded\", color = \"#f58eb4\",\n"
                "\t	 fixedsize = true, height = 1, width = 2, fontsize = 15];\n"
                "\t{rank = same;\n"
                "\t\telem0 [label = \"{[0] | {prev = %d | next = %zd } }\", color = red];\n",
            lst->prev[DUMMY_NODE_ID], lst->next[DUMMY_NODE_ID]);

    for (size_t i = 1; i < lst->capacity; i++)
    {
        fprintf(fp, "\t\telem%zd [label = \"{[%zd] ", i, i);
        char buffer[32] = "";
        if (lst->data[i])
        {
            for (size_t k = 0; k < 32 && ((char *) lst->data[i])[k] != '\0'; k++)
            {
                buffer[k] = ((char *) lst->data[i])[k];
            }
            fprintf(fp, "%s", buffer);
        }
        else 
        {
            fprintf(fp, "TRASH");
        }

        fprintf(fp, " | {prev = %d | next = %zd } }\", color = \"%s\" ];\n",
                lst->prev[i], lst->next[i], lst->prev[i] == -1 ? GREEN_COLOR :BLUE_COLOR);
    }

    fprintf(fp, "\t}\n"
                "\t{rank = max;\n"
                "\t\tinfo_node [label = \"%s[%p]\\n from %s, %s:%d\\n\\n | { <h> head = %zd | <t> tail = %zd | free = %d | cap = %zd }\", width = 3];\n"
                "\t}\n",
            lst_name, lst, file, func, line, lst->head, lst->tail, lst->free, lst->capacity);

    fprintf(fp, "\t");
    for (size_t i = 0; i < lst->capacity - 1; i++)
    {
        fprintf(fp, "elem%zd -> ", i);
    }

    fprintf(fp, "elem%zd [weight = 5, color = white];\n", lst->capacity - 1);

    fprintf(fp, "\t");
    for (size_t i = 0; i != lst->tail; i = lst->next[i])
    {
        fprintf(fp, "elem%zd -> ", i);
    }

    fprintf(fp, "elem%zd -> elem%zd [weight = 1, color = red, splines = orthro];\n"
                "\tinfo_node -> elem%zd [splines = orthro, color = blue];\n"
                "\tinfo_node -> elem%zd [splines = orthro, color = \"#fc036f\"];\n"
                "}",
            lst->tail, DUMMY_NODE_ID, lst->head, lst->tail);

    fclose(fp);
}


LstError_t list_insert(LinkedList * lst, size_t elem_id, Elem_t val)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (errors = list_vtor(lst))
    {
        printf("vtor");
        return errors;
    }

    if (lst->prev[lst->next[lst->free]] != -1)
    {
        list_resize(lst, LIST_RESIZE_EXPAND, 2);

        return errors;
    }

    if (lst->prev[elem_id] == -1)
    {
        errors |= LIST_ERROR_CANT_INSERT_TO_FREE;
        return errors;
    }

    size_t new_val_id = lst->free;
    size_t past_elem_next = lst->next[elem_id];

    lst->free = (int) lst->next[new_val_id];
    lst->data[new_val_id] = val;

    lst->next[elem_id] = new_val_id;
    lst->next[new_val_id] = past_elem_next;

    lst->prev[past_elem_next] = (int) new_val_id;
    lst->prev[new_val_id] = (int) elem_id;

    lst->size++;
    set_list_head(lst, lst->next[DUMMY_NODE_ID]);
    set_list_tail(lst, lst->prev[DUMMY_NODE_ID]);

    return errors;
}


LstError_t list_push_front(LinkedList * lst, Elem_t val)
{
    MY_ASSERT(lst);

    return list_insert(lst, lst->tail, val);
}


LstError_t list_delete(LinkedList * lst, size_t elem_id)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (errors = list_vtor(lst))
    {
        return errors;
    }

    if (lst->prev[elem_id] == -1 || elem_id == DUMMY_NODE_ID)
    {
        errors |= LIST_ERROR_CANT_DELETE_FREE;

        return errors;
    }

    int past_free = lst->free;
    size_t past_next = lst->next[elem_id];
    int past_prev = lst->prev[elem_id];

    lst->data[elem_id] = DATA_TRASH_VALUE;
    lst->free = (int) elem_id;

    lst->next[past_prev] = past_next;
    lst->prev[past_next] = past_prev;

    lst->next[elem_id] = past_free;
    lst->prev[elem_id] = -1;

    lst->size--;
    set_list_head(lst, lst->next[DUMMY_NODE_ID]);
    set_list_tail(lst, lst->prev[DUMMY_NODE_ID]);

    return errors;
}


LstError_t list_clear(LinkedList * lst)
{
    MY_ASSERT(lst);

    lst->size = 0;
    lst->free = 1;

    for (size_t i = 0; i < lst->capacity; i++)
    {
        lst->data[i] = DATA_TRASH_VALUE;
        lst->next[i] = i % (lst->capacity - 1) + 1;
        lst->prev[i] = -1;
    }
    set_list_head(lst, DUMMY_NODE_ID);
    set_list_tail(lst, DUMMY_NODE_ID);

    return 0;
}


size_t list_get_size(LinkedList * lst)
{
    MY_ASSERT(lst);

    size_t lst_size = 0;
    for (size_t i = lst->head; lst->prev[i] != -1 && i != DUMMY_NODE_ID ; i = lst->next[i])
    {
        lst_size++;
    }
    
    return lst_size;
}


static void * real_calloc(void * ptr, size_t old_size, size_t new_size)
{
    MY_ASSERT(ptr);

    char * mem_ptr = (char *) realloc(ptr, new_size);
    if (!mem_ptr)
        return mem_ptr;

    memset(mem_ptr + old_size, 0, new_size - old_size);

    return (void *) mem_ptr;
}


LstError_t list_resize(LinkedList * lst, ListResizeModes mode, size_t resize_coefficient)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;
    Elem_t * data_pointer = NULL;
    size_t * next_pointer = NULL;
    int * prev_pointer = NULL;
    size_t old_capacity = lst->capacity;
    size_t new_capacity = 0;

    if (errors = list_vtor(lst))
    {
        return errors;
    }

    switch (mode)
    {
        case LIST_RESIZE_EXPAND:
            new_capacity = old_capacity * resize_coefficient;
            break;

        case LIST_RESIZE_CONSTRICT:
            new_capacity = old_capacity / resize_coefficient;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    if (!(data_pointer = (Elem_t *) real_calloc(lst->data, old_capacity * sizeof(Elem_t), new_capacity * sizeof(Elem_t))) ||
        !(next_pointer = (size_t *) real_calloc(lst->next, old_capacity * sizeof(size_t), new_capacity * sizeof(size_t))) ||
        !(prev_pointer = (int *)    real_calloc(lst->prev, old_capacity * sizeof(int),    new_capacity * sizeof(int))))
    {
        free(lst->data);
        free(lst->prev);
        free(lst->next);

        errors |= LIST_ERROR_CANT_ALLOCATE_MEMORY;
        return errors;
    }

    lst->capacity = new_capacity;
    lst->data = data_pointer;
    lst->next = next_pointer;
    lst->prev = prev_pointer;

    int i = 0;
    int last_free = 0;
    size_t last_elem = 0;
    switch (mode)
    {
        case LIST_RESIZE_EXPAND:
            if (lst->free != -1)
            {
                for (i = lst->free; lst->prev[lst->next[i]] == -1; i = (int) lst->next[i])
                    continue;

                lst->next[i] = new_capacity - old_capacity;
            }
            else
            {
                lst->free = (int) (new_capacity - old_capacity);
            }

            for (size_t j = new_capacity - old_capacity; j < new_capacity; j++)
            {
                lst->next[j] = j % (new_capacity - 1) + 1;
                lst->prev[j] = -1;
            }

            break;

        case LIST_RESIZE_CONSTRICT:

            #if 0
                Proverka na free = -1
            #endif

            for (size_t j = lst->free; lst->prev[j] == -1; j = lst->next[j])
            {
                if (j < new_capacity)
                {
                    lst->free = (int) j;
                    last_free = (int) j;
                    break;
                }
            }

            for (size_t j = lst->free; lst->prev[j] == -1; j = lst->next[j])
            {
                if (j < new_capacity)
                {
                    lst->next[last_free] = j;
                    last_free = (int) j;
                }
            }

            last_elem = lst->head;

            for (i = (int) lst->head; i != (int) lst->tail; i = (int) lst->next[i])
            {
                if (i < (int) new_capacity)
                {
                    lst->next[last_elem] = i;
                    lst->prev[i] = (int) last_elem;
                    last_elem = i;
                }
            }

            if (i >= (int) new_capacity)
            {
                lst->tail = last_elem;
            }

            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return errors;
}


LstError_t get_elem_actual_index_by_serial_index(LinkedList * lst,
                                              size_t serial_id,
                                              int * actual_id)
{
    MY_ASSERT(lst);
    MY_ASSERT(actual_id);

    LstError_t errors = 0;

    if (errors = list_vtor(lst))
    {
        return errors;
    }

    ///
    for (size_t i = lst->next[DUMMY_NODE_ID], j = 0; i != lst->tail; i = lst->next[i], j++)
    {
        if (j == serial_id)
        {
            *actual_id = (int) i;

            return errors;
        }
    }

    *actual_id = -1;

    return errors;
}


static LstError_t list_reference_arrays_vtor(LinkedList * lst)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (!lst->next)
    {
        errors |= LIST_ERROR_INVALID_NEXT;
        return errors;
    }

    if(!lst->prev)
    {
        errors |= LIST_ERROR_INVALID_PREV;
        return errors;
    }

    bool next_check_necessity = true;
    bool prev_check_necessity = true;
    for (size_t i = 0; i < lst->capacity; i++)
    {
        if ((lst->next[i] > lst->capacity) &&
            next_check_necessity)
        {
            errors |= LIST_ERROR_INVALID_NEXT;
            next_check_necessity = false;
        }

        if ((lst->prev[i] < -1 && lst->prev[i] > (int) lst->capacity) &&
            prev_check_necessity)
        {
            errors |= LIST_ERROR_INVALID_PREV;
            prev_check_necessity = false;
        }

        if (!next_check_necessity && !prev_check_necessity)
            break;
    }

    return errors;
}


static LstError_t set_list_head(LinkedList * lst, size_t new_head_id)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (new_head_id >= lst->capacity)
    {
        errors |= LIST_ERROR_INVALID_ELEM_ID;
        return errors;
    }

    lst->next[DUMMY_NODE_ID] = new_head_id;
    lst->head                = new_head_id;

    return errors;
}


static LstError_t set_list_tail(LinkedList * lst, size_t new_tail_id)
{
    MY_ASSERT(lst);

    LstError_t errors = 0;

    if (new_tail_id >= lst->capacity)
    {
        errors |= LIST_ERROR_INVALID_ELEM_ID;
        return errors;
    }

    lst->prev[DUMMY_NODE_ID] = (int) new_tail_id;
    lst->tail                = new_tail_id;

    return errors;
}
