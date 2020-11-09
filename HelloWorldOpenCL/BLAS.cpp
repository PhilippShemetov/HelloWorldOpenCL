#include "BLAS.h"
#include <iostream>

void saxpy(int n, float a, float* x, int incx, float* y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}

void daxpy(int n, double a, double* x, int incx, double* y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}
