#pragma once
#include <random>
#include <iostream>


class Matrix
{
public:
	Matrix(const int n);
	Matrix(const Matrix& obj);
	~Matrix();
	void ArrayRNG();
	void ArrayNotRNG();
	void Print();
	float* mat;
	int size;
	friend bool operator==(const Matrix& lhs, const Matrix& rhs);
private:

};



float* TransposeMatrix(const int size, float* mat) {
	float* transposeMat = new float[size*size];
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			transposeMat[j * size + i] = mat[i * size + j];
		}
	}
	return transposeMat;
}

Matrix::Matrix(const int n)
{
	size = n;
	mat = new float[n*n];
	
	ArrayRNG();
	
}

inline Matrix::Matrix(const Matrix& obj)
{
	size = obj.size;
	mat = new float[size * size];
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i * size + j] = obj.mat[i * size + j];
		}
	}
}

Matrix::~Matrix()
{
	delete[] mat;
}

inline void Matrix::ArrayRNG()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0, 100.0);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i * size + j] = dis(gen);
		}
	}
}

inline void Matrix::ArrayNotRNG()
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			mat[i * size + j] = i*j;
		}
	}
}

inline void Matrix::Print()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
			std::cout << mat[i * size + j] << " ";
		std::cout << std::endl;
	}
}

bool operator==(const Matrix& a, const Matrix& b) {
	float eps = 0.5;
	for (int i = 0; i < a.size; i++) {
		for (int j = 0; j < a.size; j++) {
			//std::cout << a.mat[i * a.size + j] << " == " << b.mat[i * b.size + j] << "fabs =" << std::fabs(a.mat[i * a.size + j] - b.mat[i * a.size + j])
				//<< std::endl;
			if (a.mat[i * a.size + j] != b.mat[i * b.size + j] && (std::fabs(a.mat[i * a.size + j] - b.mat[i * a.size + j]) > eps)) {
				std::cout << a.mat[i * a.size + j] << " != " << b.mat[i * a.size + j] << "fabs =" << std::fabs(a.mat[i * a.size + j] - b.mat[i * a.size + j]);
				printf("False\n");
				return false;
			}

		}
	}
	printf("True\n");
	return true;
}