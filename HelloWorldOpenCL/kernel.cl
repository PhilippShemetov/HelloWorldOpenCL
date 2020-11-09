__kernel void printHello() {
	int i = get_global_id(0);
	std::cout << "I am from " << N << " block, " << M << " thread (global index: " << i << " )";
}