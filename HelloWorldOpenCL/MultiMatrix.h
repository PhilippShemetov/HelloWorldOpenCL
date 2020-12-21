#pragma once
#include <omp.h>

void MultiMatrix(int size, const float* a, const float* b, float* result);

void MultiMatrixOmp(int size, const float* a, const float* b, float* result);