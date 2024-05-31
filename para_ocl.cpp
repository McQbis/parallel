#include <iostream>
#include <cstdlib>
#include <vector>
#include <CL/cl.hpp>
#include <algorithm>

struct Item
{
    bool s = 0;
    int w, v;
};

int W, n;

Item* items;

void generateInstance()
{
    items = new Item[n];
    for(int i = 0; i < n; i++)
    {
        items[i].v = 1 + (rand()%W);
        items[i].w = 1 + (rand()%W);
    }
}

int F;

void dp()
{
    std::string code = R"(
    __kernel void knapsack(
        __global const int *weights,
        __global const int *values,
        __global int *V,
        const int i,
        const int W)
    {
        int j = get_global_id(0) + 1;

        if (j < W+1)
        {
            int idx1 = i * (W+1) + j;
            int idx2 = (i-1) * (W+1) + j;
            if(weights[i-1] > j) {
                V[idx1] = V[idx2];
            } else {
                int valueIfIncluded = V[(i-1) * (W+1) + (j - weights[i-1])] + values[i-1];
                int valueIfExcluded = V[idx2];
                V[idx1] = max(valueIfIncluded, valueIfExcluded);
            }
        }
    }
    )";

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();
    
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();
    
    cl::Context context({device});
    cl::CommandQueue queue(context, device);

    std::vector<int> weights(n), values(n), V((n+1)*(W+1), 0);
    for (int i = 0; i < n; i++) {
        weights[i] = items[i].w;
        values[i] = items[i].v;
    }
    cl::Buffer weightsBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, weights.data());
    cl::Buffer valuesBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * n, values.data());
    cl::Buffer VBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * (n+1) * (W+1), V.data());

    cl::Program program(context, code);
    program.build({device});
    cl::Kernel kernel(program, "knapsack");

    kernel.setArg(0, weightsBuffer);
    kernel.setArg(1, valuesBuffer);
    kernel.setArg(2, VBuffer);
    kernel.setArg(4, W);

    cl::NDRange global(W);
    cl::NDRange local(1);

    for (int i = 1; i <= n; i++) {
        kernel.setArg(3, i);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
        queue.finish();
    }
    queue.enqueueReadBuffer(VBuffer, CL_TRUE, 0, sizeof(int) * (n+1) * (W+1), V.data());
    F = V[n * (W+1) + W];
    int i = n;
    int j = W;
    while(i > 0 && j > 0) {
        if(V[i * (W+1) + j] > V[(i-1) * (W+1) + j]) {
            i--;
            j -= items[i].w;
            items[i].s = 1;
        } else {
            i--;
        }
    }
}

int main(){
    printf("Podaj liczbe przedmiotow:\n");
    scanf("%d", &n);
    printf("Podaj pojemnosc plecaka:\n");
    scanf("%d", &W);
    generateInstance();
    for(int i = 0; i < n; i++)
    {
        printf("Przedmiot %d wartosc %d waga %d\n", i, items[i].v, items[i].w);
    }
    dp();
    printf("\nfunkcja celu: %d\n", F);
    for(int i = 0; i < n; i++)
    {
        if(items[i].s) printf("Przedmiot %d wartosc %d waga %d\n", i, items[i].v, items[i].w);
    }
    return 0;
}