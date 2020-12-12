#pragma once
#include <random>
#include <ctime>

template <typename T>
struct TAxpyArray
{
    int size;
    int incx, incy;
    int sizeX;
    int sizeY;
    T a;
    T* x;
    T* y;
    TAxpyArray(int _n, int _incx, int _incy, T _a);
    TAxpyArray(const TAxpyArray& obj);
    ~TAxpyArray();
    void ArrayRNG();
    void ArrayNotRNG();
    friend bool operator==(const TAxpyArray& obj1, const TAxpyArray& obj2);

};

template<typename T>
TAxpyArray<T>::TAxpyArray(int _n, int _incx, int _incy, T _a)
{
    size = _n;
    incx = _incx, incy = _incy;
    sizeX = 1 + (size - 1) * abs(incx);
    sizeY = 1 + (size - 1) * abs(incy);
    a = _a;
    x = new T[sizeX];
    y = new T[sizeY];

    ArrayRNG();

}

template<typename T>
inline TAxpyArray<T>::TAxpyArray(const TAxpyArray& obj)
{
    size = obj.size;
    incx = obj.incx;
    incy = obj.incy;
    sizeX = obj.sizeX;
    sizeY = obj.sizeY;
    a = obj.a;
    x = new T[sizeX];
    y = new T[sizeY];

    for (int i = 0; i < sizeX; i++) {
        x[i] = obj.x[i];
    }

    for (int i = 0; i < sizeY; i++) {
        y[i] = obj.y[i];
    }
}

template<typename T>
TAxpyArray<T>::~TAxpyArray()
{

    delete[] x;
    delete[] y;
}

template<typename T>
void TAxpyArray<T>::ArrayRNG()
{
    std::random_device rd;
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<T> dis(0.0, 10000.0);

    for (int i = 0; i < sizeX; i++) {
        x[i] = dis(gen);
    }

    for (int i = 0; i < sizeY; i++) {
        y[i] = dis(gen);
    }
}

template<typename T>
void TAxpyArray<T>::ArrayNotRNG()
{
    for (int i = 0; i < sizeX; i++) {
        x[i] = T(i) + 0.1;
    }

    for (int i = 0; i < sizeY; i++) {
        y[i] = T(i) + 0.1;
    }
}

//template<typename T>
//bool operator==(const TAxpyArray<T>& obj1, const TAxpyArray<T>& obj2) {
//    for(int i = 0; i < s)
//}