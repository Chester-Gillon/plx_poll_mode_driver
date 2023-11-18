/* Written as part of the investigation of
  https://stackoverflow.com/questions/77504952/calling-fortran-executable-file-from-matlab-is-much-slower#comment136637363_77504952

  Simple reports the core affinity and environment variables of the process.
  Intended to look for differences in the output between being called from a command prompt or from MATLAB.
*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>


static void display_affinity (const char *const affinity_type, const DWORD affinity_mask)
{
    DWORD core;

    printf ("%s:", affinity_type);
    {
        for (core = 0; core < 64; core++)
        {
            const DWORD core_mask = 1ULL << core;

            if ((affinity_mask & core_mask) != 0)
            {
                printf (" %lu", core);
            }
        }
    }
    printf ("\n");
}

int main (int argc, char *argv[])
{
    DWORD process_affinity;
    DWORD system_affinity;
    LPCH env_vars;
    unsigned int env_var_num;
    size_t env_var_len;

    /* Report the procesws and system core affinities */
    if (GetProcessAffinityMask (GetCurrentProcess (), &process_affinity, &system_affinity))
    {
        display_affinity ("Process Affinity", process_affinity);
        display_affinity ("System Affinity", system_affinity);
    }

    /* Report all environment variables */
    env_vars = GetEnvironmentStrings ();
    if (env_vars != NULL)
    {
        printf ("\nEnvironment variables:\n");
        env_var_num = 0;
        env_var_len = strlen (env_vars);
        while (env_var_len > 0)
        {
            printf ("%s\n", env_vars);
            env_vars += (env_var_len + 1);
            env_var_len = strlen (env_vars);
        }
    }

    return EXIT_SUCCESS;
}
