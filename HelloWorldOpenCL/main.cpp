#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>
#include "BLAS.h"
#include "TAxpyArray.h"
#include <cmath>

template<typename T>
bool checkSaxpy(T* a, T* b, int sizeY) {
    T eps = 0.005;
    for (int i = 0; i < sizeY; i++) {
        if (a[i] != b[i] && (std::fabs(a[i]-b[i]) > eps)) {
            return false;
        }
    }
    return true;
}

void outputPlatform() {
    cl_uint platformCount = 0;

    clGetPlatformIDs(0, nullptr, &platformCount);

    cl_device_id* devices = new cl_device_id[5];

    cl_platform_id* platform = new cl_platform_id[platformCount];

    clGetPlatformIDs(platformCount, platform, nullptr);

    for (cl_uint i = 0; i < platformCount; ++i) {
        char platformName[128];
        char openCLVersion[128];
        char deviceName[128];
        cl_uint deviceCount = 0;

        clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, 5, devices, NULL);

        clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);

        clGetPlatformInfo(platform[i], CL_PLATFORM_NAME,
            128, platformName, nullptr);

        clGetPlatformInfo(platform[i], CL_PLATFORM_VERSION,
            128, openCLVersion, nullptr);

        std::cout << "Platform: "<<platformName << std::endl;

        std::cout << "OpenCL ver.: " << openCLVersion << std::endl;

        for (cl_uint t = 0; t < deviceCount; t++) {
            
            clGetDeviceInfo(devices[t], CL_DEVICE_NAME, 128, deviceName, nullptr);

            std::cout << "Device [" << t << "]: "  << deviceName << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    delete[] platform;

}

void printBlockAndGroup() {


    std::fstream f("kernel.cl", std::ios::in);
    std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    const char* source = code.c_str();

    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;
     
    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[0];
        delete[] platforms;

    }

    //Получаем устройство(GTX 1050TI на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(GTX 1050TI на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(source) };

    

    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &source, srclen, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "printHello", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;

    //
    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }

    size_t global_work = local_work * 4;

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }

    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }

    //clEnqueueReadBuffer(queue, nullptr, CL_TRUE, 0, 0, nullptr, 0, nullptr, nullptr);



    clReleaseContext(context);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
}

void AddVectors() {
    const char* source =
        "__kernel void printArray (__global int* array) {          \n"\
        "   int globalID = get_global_id(0);                       \n"\
        "   array[globalID] += globalID;                           \n"\
        "}                                                         \n";

    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;

    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[0];
        delete[] platforms;

    }

    //Получаем устройство(GTX 1050TI на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(GTX 1050TI на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device,
                                         nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue = 
        clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(source) };



    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &source, srclen,
                                                   &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "printArray", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;

    
    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
                                   sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }


    size_t global_work = local_work * 4;

    std::vector<int> arr(global_work);

    cl_mem buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * arr.size(),nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    err = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, sizeof(int) * arr.size(), arr.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clSetKernelArg: " << err << std::endl;
    }

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }


    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clFinish: " << err << std::endl;
    }

    err = clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, sizeof(int) * arr.size(), arr.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueReadBuffer: " << err << std::endl;
    }
    for (auto& x : arr) {
        std::cout << x << " ";
    }

    clReleaseMemObject(buffer);
    clReleaseContext(context);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
}

void cpuBLAS() {

    TAxpyArray<float> fArray(25600000, 3, 2, 1.3);
    
    TAxpyArray<double> dArray(25600000, 3, 2, 1.3);
    
    /*TAxpyArray<double> dArray;*/
    
    double start = omp_get_wtime();
    saxpy(fArray.size, fArray.a,fArray.x,fArray.incx, fArray.y, fArray.incy);
    double finish = omp_get_wtime();
    printf("Time of saxpy on CPU %f in sec\n", finish - start);
    double start2 = omp_get_wtime();
    daxpy(dArray.size, dArray.a, dArray.x, dArray.incx, dArray.y, dArray.incy);
    double finish2 = omp_get_wtime();
    printf("Time of daxpy on CPU %f in sec\n", finish2 - start2);
    
    
    
}

void cpuGraphicSaxpy() {

    std::fstream f("BLAS.cl", std::ios::in);
    std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    const char* sourceCode = code.c_str();



    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;

    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[1];
        delete[] platforms;

    }

    //Получаем устройство(HD Graphics 610 на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(HD Graphics 610 на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device,
        nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue =
        clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(sourceCode) };



    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &sourceCode, srclen,
        &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "saxpy", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;


    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }


    size_t global_work = local_work * 100000;

    TAxpyArray<float> fArray(global_work, 3, 2, 1.3);

    TAxpyArray<float> fArray2(fArray);

    saxpy(fArray2.size, fArray2.a, fArray2.x, fArray2.incx, fArray2.y, fArray2.incy);

    cl_mem bufferX = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * fArray.sizeX, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    cl_mem bufferY = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * fArray.sizeY, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    //Заполняем буфер
    err = clEnqueueWriteBuffer(queue, bufferX, CL_TRUE, 0, sizeof(float) * fArray.sizeX, fArray.x, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clEnqueueWriteBuffer(queue, bufferY, CL_TRUE, 0, sizeof(float) * fArray.sizeY, fArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clSetKernelArg(kernel, 0, sizeof(unsigned int), &fArray.size);
    err = clSetKernelArg(kernel, 1, sizeof(float), &fArray.a);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferX);
    err = clSetKernelArg(kernel, 3, sizeof(int), &fArray.incx);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufferY);
    err = clSetKernelArg(kernel, 5, sizeof(int), &fArray.incy);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clSetKernelArg: " << err << std::endl;
    }

    double start = omp_get_wtime();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }


    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clFinish: " << err << std::endl;
    }
    double finish = omp_get_wtime();
    printf("Time of cpuGraphicsSaxpy %f in sec\n", finish - start);

    err = clEnqueueReadBuffer(queue, bufferY, CL_TRUE, 0, sizeof(float) * fArray.sizeY, fArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueReadBuffer: " << err << std::endl;
    }
    /*for (int i = 0; i < dArray.sizeY; i++) {
        std::cout << dArray.y[i] << " " << dArray2.y[i] << std::endl;
    }*/

    if (checkSaxpy<float>(fArray.y, fArray2.y, fArray.sizeY)) {
        std::cout << "Correct work cpuGraphicsSaxpy\n";
    }
    else {
        std::cout << "Doesn't correct work cpuGraphicsSaxpy\n";
    }



    clReleaseContext(context);
    clReleaseDevice(device);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);

}

void cpuGraphicDaxpy() {

    std::fstream f("BLAS.cl", std::ios::in);
    std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    const char* sourceCode = code.c_str();



    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;

    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[1];
        delete[] platforms;

    }

    //Получаем устройство(GTX 1050TI на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(GTX 1050TI на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device,
        nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue =
        clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(sourceCode) };



    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &sourceCode, srclen,
        &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "daxpy", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;


    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }


    size_t global_work = local_work * 100000;

    TAxpyArray<double> dArray(global_work, 3, 2, 1.3);

    TAxpyArray<double> dArray2(dArray);

    daxpy(dArray2.size, dArray2.a, dArray2.x, dArray2.incx, dArray2.y, dArray2.incy);

    cl_mem bufferX = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(double) * dArray.sizeX, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    cl_mem bufferY = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * dArray.sizeY, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    //Заполняем буфер
    err = clEnqueueWriteBuffer(queue, bufferX, CL_TRUE, 0, sizeof(double) * dArray.sizeX, dArray.x, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clEnqueueWriteBuffer(queue, bufferY, CL_TRUE, 0, sizeof(double) * dArray.sizeY, dArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clSetKernelArg(kernel, 0, sizeof(unsigned int), &dArray.size);
    err = clSetKernelArg(kernel, 1, sizeof(double), &dArray.a);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferX);
    err = clSetKernelArg(kernel, 3, sizeof(int), &dArray.incx);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufferY);
    err = clSetKernelArg(kernel, 5, sizeof(int), &dArray.incy);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clSetKernelArg: " << err << std::endl;
    }

    double start = omp_get_wtime();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }


    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clFinish: " << err << std::endl;
    }
    double finish = omp_get_wtime();
    printf("Time of gpuDaxpy %f in sec\n", finish - start);

    err = clEnqueueReadBuffer(queue, bufferY, CL_TRUE, 0, sizeof(double) * dArray.sizeY, dArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueReadBuffer: " << err << std::endl;
    }
    /*for (int i = 0; i < dArray.sizeY; i++) {
        std::cout << dArray.y[i] << " " << dArray2.y[i] << std::endl;
    }*/

    if (checkSaxpy<double>(dArray.y, dArray2.y, dArray.sizeY)) {
        std::cout << "Correct work cpuGraphicsDaxpy\n";
    }
    else {
        std::cout << "Doesn't correct work cpuGraphicsDaxpy\n";
    }



    clReleaseContext(context);
    clReleaseDevice(device);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);

}

void gpuSaxpy() {

    std::fstream f("BLAS.cl", std::ios::in);
    std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    const char* sourceCode = code.c_str();



    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;

    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[0];
        delete[] platforms;

    }

    //Получаем устройство(GTX 1050TI на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(GTX 1050TI на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device,
        nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue =
        clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(sourceCode) };



    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &sourceCode, srclen,
        &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "saxpy", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;


    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }


    size_t global_work = local_work * 100000;

    TAxpyArray<float> fArray(global_work, 3, 2, 1.3);

    TAxpyArray<float> fArray2(fArray);

    saxpy(fArray2.size, fArray2.a, fArray2.x, fArray2.incx, fArray2.y, fArray2.incy);

    cl_mem bufferX = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * fArray.sizeX, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    cl_mem bufferY = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * fArray.sizeY, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    //Заполняем буфер
    err = clEnqueueWriteBuffer(queue, bufferX, CL_TRUE, 0, sizeof(float) * fArray.sizeX, fArray.x, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clEnqueueWriteBuffer(queue, bufferY, CL_TRUE, 0, sizeof(float) * fArray.sizeY, fArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clSetKernelArg(kernel, 0, sizeof(unsigned int), &fArray.size);
    err = clSetKernelArg(kernel, 1, sizeof(float), &fArray.a);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferX);
    err = clSetKernelArg(kernel, 3, sizeof(int), &fArray.incx);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufferY);
    err = clSetKernelArg(kernel, 5, sizeof(int), &fArray.incy);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clSetKernelArg: " << err << std::endl;
    }

    double start = omp_get_wtime();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }


    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clFinish: " << err << std::endl;
    }
    double finish = omp_get_wtime();
    printf("Time of gpuSaxpy %f in sec\n", finish - start);

    err = clEnqueueReadBuffer(queue, bufferY, CL_TRUE, 0, sizeof(float) * fArray.sizeY, fArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueReadBuffer: " << err << std::endl;
    }
    /*for (int i = 0; i < dArray.sizeY; i++) {
        std::cout << dArray.y[i] << " " << dArray2.y[i] << std::endl;
    }*/

    if (checkSaxpy<float>(fArray.y, fArray2.y, fArray.sizeY)) {
        std::cout << "Correct work saxpy GPU\n";
    }
    else {
        std::cout << "Doesn't correct work saxpy GPU\n";
    }



    clReleaseContext(context);
    clReleaseDevice(device);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);

}

void gpuDaxpy() {

    std::fstream f("BLAS.cl", std::ios::in);
    std::string code((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    const char* sourceCode = code.c_str();



    cl_uint numPlatforms = 0;
    cl_int err = CL_SUCCESS;

    clGetPlatformIDs(0, nullptr, &numPlatforms);

    cl_platform_id platform = nullptr;

    //Получаем число доступных платформ 
    if (numPlatforms > 0) {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];

        clGetPlatformIDs(numPlatforms, platforms, nullptr);

        platform = platforms[0];
        delete[] platforms;

    }

    //Получаем устройство(GTX 1050TI на моем ПК)
    cl_device_id* devices = new cl_device_id[1];
    cl_device_id device = nullptr;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, devices, NULL);

    device = devices[0];

    //Создаем контекст, передав устройство(GTX 1050TI на моем ПК)
    cl_context context = clCreateContext(nullptr, 1, &device,
        nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in context: " << err << std::endl;
    }

    //Создание очереди команд для взаймодествия между контекстом и устройством
    cl_command_queue queue =
        clCreateCommandQueueWithProperties(context, device, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateCommandQueueWithProperties: " << err << std::endl;
    }

    //Получаем размер текста в ядре
    size_t srclen[] = { strlen(sourceCode) };



    //Создаем объект программы из исходного кода
    cl_program program = clCreateProgramWithSource(context, 1, &sourceCode, srclen,
        &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateProgramWithSource: " << err << std::endl;
    }

    //Компилируем и собираем ядро
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clBuildProgram: " << err << std::endl;
    }

    //Создание объекта ядра по имени функции ядра в исходном коде
    cl_kernel kernel = clCreateKernel(program, "daxpy", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateKernel: " << err << std::endl;
    }

    size_t local_work;


    err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t), &local_work, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clGetKernelWorkGroupInfo: " << err << std::endl;

    }


    size_t global_work = local_work * 100000; 

    TAxpyArray<double> dArray(global_work, 3, 2, 1.3);

    TAxpyArray<double> dArray2(dArray);

    daxpy(dArray2.size, dArray2.a, dArray2.x, dArray2.incx, dArray2.y, dArray2.incy);

    cl_mem bufferX = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(double) * dArray.sizeX, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    cl_mem bufferY = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * dArray.sizeY, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clCreateBuffer: " << err << std::endl;
    }

    //Заполняем буфер
    err = clEnqueueWriteBuffer(queue, bufferX, CL_TRUE, 0, sizeof(double) * dArray.sizeX, dArray.x, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clEnqueueWriteBuffer(queue, bufferY, CL_TRUE, 0, sizeof(double) * dArray.sizeY, dArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueWriteBuffer: " << err << std::endl;
    }

    err = clSetKernelArg(kernel, 0, sizeof(unsigned int), &dArray.size);
    err = clSetKernelArg(kernel, 1, sizeof(double), &dArray.a);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferX);
    err = clSetKernelArg(kernel, 3, sizeof(int), &dArray.incx);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufferY);
    err = clSetKernelArg(kernel, 5, sizeof(int), &dArray.incy);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clSetKernelArg: " << err << std::endl;
    }
    
    double start = omp_get_wtime();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueNDRangeKernel: " << err << std::endl;
    }


    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clFinish: " << err << std::endl;
    }
    double finish = omp_get_wtime();
    printf("Time of gpuDaxpy %f in sec\n", finish - start);

    err = clEnqueueReadBuffer(queue, bufferY, CL_TRUE, 0, sizeof(double) * dArray.sizeY, dArray.y, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Error in clEnqueueReadBuffer: " << err << std::endl;
    }
    /*for (int i = 0; i < dArray.sizeY; i++) {
        std::cout << dArray.y[i] << " " << dArray2.y[i] << std::endl;
    }*/

    if (checkSaxpy<double>(dArray.y, dArray2.y, dArray.sizeY)) {
        std::cout << "Correct work daxpy GPU\n";
    }
    else {
        std::cout << "Doesn't correct work daxpy GPU\n";
    }



    clReleaseContext(context);
    clReleaseDevice(device);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);

}

void cpuBLAS_OMP() {

    
    TAxpyArray<float> fArray(25600000, 3, 2, 1.3);

    TAxpyArray<double> dArray(25600000, 3, 2, 1.3);

    TAxpyArray<float> fArray2(fArray);

    TAxpyArray<double> dArray2(dArray);

    saxpy(fArray2.size, fArray2.a, fArray2.x, fArray2.incx, fArray2.y, fArray2.incy);
    daxpy(dArray2.size, dArray2.a, dArray2.x, dArray2.incx, dArray2.y, dArray2.incy);
    double start = omp_get_wtime();
    saxpy_omp(fArray.size, fArray.a, fArray.x, fArray.incx, fArray.y, fArray.incy);
    double finish = omp_get_wtime();
    printf("Time of OMP saxpy on CPU %f in sec\n", finish - start);
    double start2 = omp_get_wtime();
    daxpy_omp(dArray.size, dArray.a, dArray.x, dArray.incx, dArray.y, dArray.incy);
    double finish2 = omp_get_wtime();
    printf("Time of OMP daxpy on CPU %f in sec\n", finish2 - start2);
    if (checkSaxpy(fArray.y, fArray2.y, fArray.sizeY)) {
        std::cout << "Correct work saxpy OMP\n";
    }
    else {
        std::cout << "Doesn't correct work saxpy OMP\n";
    }
    if (checkSaxpy(dArray.y, dArray2.y, dArray.sizeY)) {
        std::cout << "Correct work daxpy OMP\n";
    }
    else {
        std::cout << "Doesn't correct work daxpy OMP\n";
    }
}

int main() {

    int x = 0;
    while (1) {
        std::cout << "Selection Menu: " << std::endl;
        std::cout << "-----LAB_1-----" << std::endl;
        std::cout << "1) Print platform and devices." << std::endl;
        std::cout << "2) Print phrase." << std::endl;
        std::cout << "3) Print array." << std::endl;
        std::cout << "-----LAB_2(WIP)-----" << std::endl;
        std::cout << "4)BLAS Function CPU" << std::endl;
        std::cout << "5)Saxpy Function GPU" << std::endl;
        std::cout << "6)Daxpy Function GPU" << std::endl;
        std::cout << "7)OMP BLAS Function CPU" << std::endl;
        std::cout << "8)Result" << std::endl;
        std::cout << "-----LAB_3-----" << std::endl;
        std::cout << "9)Clear console" << std::endl;
        std::cout << "10) Exit." << std::endl;
        std::cin >> x;
        switch (x)
        {
        case 1:
            system("cls");
            outputPlatform();
            break;
        case 2:
            system("cls");
            printBlockAndGroup();
            break;
        case 3:
            system("cls");
            AddVectors();
            break;
        case 4:
            system("cls");
            cpuBLAS();
            break;
        case 5:
            system("cls");
            gpuSaxpy();
            break;
        case 6:
            system("cls");
            gpuDaxpy();
            break;
        case 7:
            system("cls");
            cpuBLAS_OMP();
            break;
        case 8:
            system("cls");
            cpuBLAS();
            cpuGraphicSaxpy();
            cpuGraphicDaxpy();
            gpuSaxpy();
            gpuDaxpy();
            cpuBLAS_OMP();
            break;
        case 9:
            system("cls");
            break;
        default:
            return 0;
        }
    }
    
    

    

    
    
}


