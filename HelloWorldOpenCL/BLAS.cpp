#include "BLAS.h"
#include <iostream>
#include <omp.h>

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

void saxpy_omp(int n, float a, float* x, int incx, float* y, int incy) {
#pragma omp parallel for num_threads(4)
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}

void daxpy_omp(int n, double a, double* x, int incx, double* y, int incy) {
#pragma omp parallel for num_threads(4)
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}
