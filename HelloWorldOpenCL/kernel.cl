__kernel void printArray (__global int* array) {
        int globalID = get_global_id(0);
        array[globalID] += globalID;
}

__kernel void printHello () {
    int globalID = get_global_id(0);
    int localID = get_local_id(0);
    int groupID = get_group_id(0);
    //if(groupID == 0)
    printf("I'm from %lu block, %lu thread (global index: %lu)\n",groupID,localID,globalID);
}