#include "MultiMatrix.h"
#include <iostream>

float* MultiMatrix(int size, float* a, float* b)
{
    float* result = new float[size * size];
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            result[i * size + j] = 0.0;
            for (int k = 0; k < size; k++) {
                result[i * size + j] += a[i * size + k] * b[k * size + j];
            }
        }
        
    }

    return result;
    
}
