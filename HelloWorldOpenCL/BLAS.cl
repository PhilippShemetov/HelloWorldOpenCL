__kernel void saxpy (const unsigned int n, float a,
                    __global float* x, int incx, __global float* y, int incy) {
    
    int i = get_global_id(0);
    if (i < n) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
}

__kernel void daxpy (const unsigned int n, double a, __global double* x,
                    int incx, __global double* y, int incy) {
    int i = get_global_id(0);
    if (i < n) {
        y[i * incy] = y[i * incy] + a * x[i * incx];
    }
} 