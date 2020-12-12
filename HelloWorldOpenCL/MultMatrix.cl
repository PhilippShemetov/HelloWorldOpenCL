__kernel void SimpleMultiMatrix (const unsigned int n, __global float* A, __global float* B, __global float* C) {
    
    
    int gid_0 = get_global_id(0);
    int gid_1 = get_global_id(1);
    int groupID1 = get_group_id(0);
    int groupID2 = get_group_id(1);
    float value = 0.0;
    //if(groupID1 == 0 && groupID2 == 0)
    //printf("block:(%i,%i) - gid:(%i,%i)\n",groupID1,groupID2,gid_0,gid_1);
    for(int k = 0; k < n; k++){
        value += A[gid_0 * n + k] * B[k * n + gid_1];
	}
    C[gid_0 * n + gid_1] = value;

}