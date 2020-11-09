#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BLAS.h"
#include "TAxpyArray.h"




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
    const char* source = 
        "__kernel void printHello () {                 \n"\
        "   int globalID = get_global_id(0);           \n"\
        "   int localID = get_local_id(0);             \n"\
        "   int groupID = get_group_id(0);             \n"\
        "   //if(groupID == 0)            \n"\
        "   printf(\"I'm from %lu block, %lu thread (global index: %lu)\\n\",groupID,localID,globalID);                   \n"\
        "}                                             \n";

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
    clReleaseDevice(device);
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

    clReleaseContext(context);
    clReleaseDevice(device);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
}

void cpuBLAS() {

    TAxpyArray<float> fArray(1000,3,3,1);
    TAxpyArray<double> dArray(1000, 3, 3, 1);
    
    /*TAxpyArray<double> dArray;*/
    for (int i = 0; i < fArray.sizeY; i++) {
        std::cout << fArray.y[i] << " ";
        std::cout << fArray.x[i] << " ";
        std::cout << std::endl;
    }
    
    saxpy(fArray.size, fArray.a,fArray.x,fArray.incx, fArray.y, fArray.incy);
    daxpy(dArray.size, dArray.a, dArray.x, dArray.incx, dArray.y, dArray.incy);
    for (int i = 0; i < fArray.sizeY; i++) {
        std::cout << fArray.y[i] << " ";
    }
}

void gpuBLAS() {

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
        std::cout << "5)BLAS Function GPU" << std::endl;
        std::cout << "6) Exit." << std::endl;
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
            gpuBLAS();
            break;
        default:
            return 0;
        }
    }
    
    

    

    
    
}


