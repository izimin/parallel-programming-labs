#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdio>
#include <curand_kernel.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thrust/device_vector.h>

const int COUNT_OF_THREADS = 128;
const int COUNT_OF_DOTS = 10000000;
const double a = 0;
const double b = M_PI / 2.;
const double yMin = 0;
const double yMax = 1;

double fI() { return sin(b) - sin(a); }

__global__ void mcKernel(const int countOfIterations, int* counts, const double a, const double b, const double yMin, const double yMax){
    int i = threadIdx.x;
    curandStatePhilox4_32_10_t  state;
    curand_init(i, 0, 0, &state);
    for (int j = 0; j < countOfIterations; j++)
    {
        double2 dot = curand_uniform2_double(&state);
        if (dot.y * yMax - yMin <= cos(dot.x * b - a))
            counts[i]++;
    }
}

int main()
{
    cudaSetDevice(0);
    thrust::device_vector<int> counts(COUNT_OF_THREADS);
    int* rawCounts = thrust::raw_pointer_cast(counts.data());
    cudaEvent_t start, stop;
    float elapsedTime;
    cudaEventCreate(&start);
    cudaEventRecord(start, 0);
    dim3 threadsPerBlock(COUNT_OF_THREADS);
    mcKernel <<<1, threadsPerBlock >> > (COUNT_OF_DOTS / COUNT_OF_THREADS, rawCounts, a, b, yMin, yMax);
    int countOfInnerPoints = thrust::reduce(counts.begin(), counts.end(), (int)0, thrust::plus<int>());
    cudaEventCreate(&stop);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    printf("Calculated integral: %f;\nPrecise integral: %f.\n", (b - a) * (yMax - yMin) * countOfInnerPoints / COUNT_OF_DOTS, fI());
    printf("Time of calculation: %f seconds\n", elapsedTime / 1000);
    cudaDeviceReset();
    system("pause");
    return 0;
}
