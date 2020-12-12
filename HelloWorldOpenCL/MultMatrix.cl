__kernel void SimpleMultiMatrix (const unsigned int n, __global float* A, __global float* B, __global float* C) {
    
    int gid_0 = get_global_id(0);
    int gid_1 = get_global_id(1);
    float value = 0.0;
    for(int k = 0; k < n; k++){
        vaule += A[gid_0 * n + k] * B[k * n + gid_1];
	}
    C[gid_0 * n + gid_1] = value;

}