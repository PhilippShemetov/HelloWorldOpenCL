__kernel void saxpy (const unsigned int n, float a,
                    __global float* x, int incx, __global float* y, int incy) {
    printf("hello");
    int i = get_global_id(0);
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}

__kernel void daxpy (const unsigned int n, double a, __global double* x,
                    int incx, __global double* y, int incy) {
    for (int i = 0; i < n; i++) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
} 