#include "MultiMatrix.h"
#include <iostream>
#include <omp.h>

void MultiMatrix(int size, const float* a, const float* b, float* result)
{
    
    int i, j, k;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            float value = 0.0;
            for (k = 0; k < size; k++) {
                value += a[i * size + k] * b[k * size + j];
                //printf("i: %i, j: %i, k: %i", i, j, k);
            }
            result[i * size + j] = value;
            
        }
        
    }
    
}

void MultiMatrixOmp(int size, const float* a, const float* b, float* result)
{
#pragma omp parallel
{
        int i, j, k;
        #pragma omp for
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                float value = 0.0;
                for (k = 0; k < size; k++) {
                    value += a[i * size + k] * b[k * size + j];
                }
                result[i * size + j] = value;
            }
        }

}
}
