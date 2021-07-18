
#include "test_lib.h"

typedef struct
{
    int a;
    int b;
    float c;
    int ints[8];
} data_t;

typedef struct
{
    float floats[8];
} vec_t;

size_t get_test_data_size (void)
{
    data_t data = {0};
    vec_t vec = {.floats[7] = 7.65431};
    data.a++;
    data.c = vec.floats[7];
    
    return sizeof (data);
}
