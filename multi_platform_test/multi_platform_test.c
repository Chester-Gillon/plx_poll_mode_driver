#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>

#ifdef _WIN32
#include <fileapi.h>
#endif

#ifdef MATLAB_MEX_FILE
#include <mex.h>
#endif


int main (int argc, char *argv[])
{
    struct timespec res;
    struct timespec start;
    struct timespec end;
    struct timespec now;
    int rc;
    bool expired;

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
#if defined(_WIN32)
    const DWORD status = GetFullPathName (current_dir, sizeof (resolved_path_buffer), resolved_path_buffer, NULL);
    resolved_path = (status != 0) ? resolved_path_buffer : NULL;
#else
    resolved_path = realpath (current_dir, resolved_path_buffer);
#endif
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
