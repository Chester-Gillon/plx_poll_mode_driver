#include "log_data.h"

#include <mex.h>
#include <matrix.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    log_data_t *const log_data = malloc (sizeof (log_data_t));
    
    float test_pattern = 1.0;
    for (int row = 0; row < 1024; row++)
    {
        for (int col = 0; col < 16; col++)
        {
            log_data->h[row][col] = test_pattern;
            log_data->i[row][col] = -test_pattern;
            test_pattern += 0.01;
        }
    }
    
    if (nlhs == 1)
    {
        *plhs = mxCreateNumericMatrix (16, 1024, mxSINGLE_CLASS, mxCOMPLEX);
        mxComplexSingle *const complex_data = mxGetComplexSingles (*plhs);
        
        for (int row = 0; row < 1024; row++)
        {
            for (int col = 0; col < 16; col++)
            {
                complex_data[(row * 16) + col].real = log_data->h[row][col];
                complex_data[(row * 16) + col].imag = log_data->i[row][col];
            }
        }
    }
    
    free (log_data);
}