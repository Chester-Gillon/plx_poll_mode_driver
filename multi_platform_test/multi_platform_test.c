#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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

    return EXIT_SUCCESS;
}
