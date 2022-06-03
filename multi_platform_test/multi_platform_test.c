#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>

#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif

#include "test_lib.h"

/* Create a realpath replacement macro for when compiling under mingw
 * Based upon https://stackoverflow.com/questions/45124869/cross-platform-alternative-to-this-realpath-definition
 */
#ifdef WIN32
    #define realpath(N,R) _fullpath((R),(N),PATH_MAX)
#endif

typedef struct
{
    size_t matrix_size;
} scalar_t;

typedef struct
{
    float a[64][1024];
    float b[1024][64];
} matrix_t;

matrix_t global_matrix = {.b[0][0]=1.234};

int main (int argc, char *argv[])
{
    struct timespec res;
    struct timespec start;
    struct timespec end;
    struct timespec now;
    int rc;
    bool expired;
    volatile scalar_t dimension_vol = {.matrix_size = sizeof(matrix_t)};
    scalar_t dimension = {.matrix_size = sizeof(matrix_t)};
    matrix_t *const matrix_a = calloc(dimension.matrix_size + get_test_data_size (),1);
    matrix_a->a[0][0] = 1024;
    matrix_a->b[1023][63] = 1025;

    rc = clock_getres (CLOCK_REALTIME, &res);
    if (rc == 0)
    {
        printf ("CLOCK_REALTIME res.tv_sec=%ld res.tv_nsec=%ld\n", res.tv_sec, res.tv_nsec);
    }
    else
    {
        printf ("clock_getres (CLOCK_REALTIME) failed rc=%d\n", rc);
    }

    rc = clock_getres (CLOCK_MONOTONIC, &res);
    if (rc == 0)
    {
        printf ("CLOCK_MONOTONIC res.tv_sec=%ld res.tv_nsec=%ld\n", res.tv_sec, res.tv_nsec);

        rc = clock_gettime (CLOCK_MONOTONIC, &start);
        end = start;
        end.tv_sec++;
        expired = false;
        while ((rc == 0) && (!expired))
        {
            rc = clock_gettime (CLOCK_MONOTONIC, &now);
            expired = (now.tv_sec >= end.tv_sec) && (now.tv_nsec >= end.tv_nsec);
        }

        if (rc == 0)
        {
            printf ("Used clock_gettime (CLOCK_MONOTONIC) to delay for one second\n");
        }
        else
        {
            printf ("clock_gettime (CLOCK_MONOTONIC) failed rc=%d\n", rc);
        }
    }
    else
    {
        printf ("clock_getres (CLOCK_MONOTONIC) failed rc=%d\n", rc);
    }
    
    const char *const current_dir = ".";
    char resolved_path_buffer[PATH_MAX];
    char *resolved_path;
    resolved_path = realpath (current_dir, resolved_path_buffer);
    if (resolved_path != NULL)
    {
        printf ("Current directory is \"%s\"\n", resolved_path);
        printf ("\nCurrent directory listing:\n");
        DIR *dir = opendir (resolved_path);
        if (dir != NULL)
        {
            struct dirent *entry = readdir (dir);
            while (entry != NULL)
            {
                printf ("  %s\n", entry->d_name);
                entry = readdir (dir);
            }
            closedir (dir);
        }
    }
    else
    {
        printf ("realpath failed");
    }
    
    free (matrix_a);

    return EXIT_SUCCESS;
}

#ifdef MATLAB_MEX_FILE
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int argc = 1;
    char *argv[] = {"multi_platform_test"};
    main (argc, argv);
}
#endif
