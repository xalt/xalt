#include <iostream>
#include <cmath>
#include <string>
#include <sys/time.h>

#include <cuda.h>
#include <cuda_runtime.h>

/* simple timer that stores multiple times 
   and can print them afterwards.
   Does not support nested timing calls. 
*/
class Timer
{
public:
    Timer(): n(0) { }
    void start(std::string label) 
    { 
        if (n < 20) { labels[n] = label; gettimeofday(&times[2*n], NULL); }
        else { std::cerr << "No more timers, " << label << " will not be timed." << std::endl; }
    }

    void stop() { gettimeofday(&times[2*n+1], NULL); n++;}
    void reset() { n=0; }
    void print();
private:
    std::string labels[20];
    timeval times[40];
    int n;
};

void Timer::print()
{
    std::cout << std::endl;
    std::cout << "Action           ::    time/s     Time resolution = " << 1.f/(float)CLOCKS_PER_SEC << std::endl;
    std::cout << "------" << std::endl;
    for (int i=0; i < n; ++i)
    {
        time_t seconds = times[2*i+1].tv_sec - times[2*i+0].tv_sec;
        suseconds_t ms = times[2*i+1].tv_usec - times[2*i+0].tv_usec;
        if (ms < 0) { ms += 1000000; seconds--; }
        std::cout << labels[i] << " ::    " << (float)seconds + ms/1000000.f << std::endl;
    }
}

void initialize( float* x, int n )
{
    int n2 = n+2;
    for (int i=0; i<n2; ++i)
        for (int j=0; j<n2; ++j)
            x[i*n2 + j] = random() / (float) RAND_MAX;
}

void smooth( float* y, float* x, int n, float a, float b, float c )
{
    int n2 = n+2;
    for (int i=1; i<=n; ++i)
        for (int j=1; j<=n; ++j)
            y[i*n2 + j] = a * (x[(i-1)*n2 + (j-1)] + x[(i-1)*n2 + (j+1)] + x[(i+1)*n2 + (j-1)] + x[(i+1)*n2 + (j+1)]) 
                + b * (x[i*n2+(j-1)] + x[i*n2+(j+1)] + x[(i-1)*n2 + j] + x[(i+1)*n2 + j])
                + c * x[i*n2 +j];
}

__global__ void smooth_gpu( float* y, float* x, int n, float a, float b, float c )
{
    int n2 = n+2, i, j;
    
    i = blockDim.x * blockIdx.x + threadIdx.x + 1;  // add one to start from index 1
    j = blockDim.y * blockIdx.y + threadIdx.y + 1;
    
    y[i*n2 + j] = a * (x[(i-1)*n2 + (j-1)] + x[(i-1)*n2 + (j+1)] + x[(i+1)*n2 + (j-1)] + x[(i+1)*n2 + (j+1)]) 
        + b * (x[i*n2+(j-1)] + x[i*n2+(j+1)] + x[(i-1)*n2 + j] + x[(i+1)*n2 + j])
        + c * x[i*n2 +j];
}


void count( float* x, int n, float t, int &nbx )
{
    nbx = 0;
    int n2 = n+2;
    for (int i=1; i <= n; ++i)
        for (int j=1; j <= n; ++j)
            if (x[i*n2 + j] < t) 
                ++nbx;
}


int main( int argc, char** argv )
{
    int n,nbx,nby,nbyh;
    float a,b,c,
        t,diff_y_yh,
        *x, *y, *y_h,
        *x_d, *y_d;
    Timer timer;

    /* n - number of elements in one direction */
    //n = 1 << 14;  /* 2^14 == 16384 */
    n = 1<<12;

    /* a,b,c - smoothing constants */
    a = 0.05;
    b = 0.1;
    c = 0.4;

    /* t - threshold */
    t = 0.1;

    /* allocate x */
    timer.start("CPU: Alloc-X    ");
    x = new float[(n+2)*(n+2)];
    timer.stop();

    /* allocate y and y_h */
    timer.start("CPU: Alloc-Y&Y_H");
    y = new float[(n+2)*(n+2)];
    y_h = new float[(n+2)*(n+2)];
    timer.stop();

    /* allocate device input array x_d */
    timer.start("GPU: Alloc-X_D  ");
    cudaMalloc((void**) &x_d, (n+2)*(n+2)*sizeof(float)); 
    timer.stop();

    /* allocate device output array y_d */
    timer.start("GPU: Alloc-Y_D  ");
    cudaMalloc((void**) &y_d, (n+2)*(n+2)*sizeof(float)); 
    timer.stop();
    
    /* initialize x */
    timer.start("CPU: Init-X     ");
    initialize(x, n);
    timer.stop();

    /* smooth x into y */
    timer.start("CPU: Smooth     ");
    smooth(y, x, n, a, b, c);
    timer.stop();
    
    /* copy x_h to x_d on GPU */
    timer.start("GPU: Copy-in    ");
    cudaMemcpy(x_d, x, (n+2)*(n+2)*sizeof(float), cudaMemcpyHostToDevice);
    timer.stop();

    /* launch smooth() on GPU */
    int nBlocks = 16;
    dim3 blockSize(nBlocks, nBlocks);
    dim3 gridSize(n/nBlocks, n/nBlocks);
    timer.start("GPU: Smooth     ");
    smooth_gpu<<<gridSize,blockSize>>>(y_d, x_d, n, a, b, c);
    cudaDeviceSynchronize();
    timer.stop();

    /* copy y_d to y_h on CPU */
    timer.start("GPU: Copy-out   ");
    cudaMemcpy(y_h, y_d, (n+2)*(n+2)*sizeof(float), cudaMemcpyDeviceToHost);
    timer.stop();

    /* count elements in first array */
    timer.start("CPU: Count-X    ");
    count(x, n, t, nbx);
    timer.stop();

    /* count elements in second array */
    timer.start("CPU: Count-Y    ");
    count(y, n, t, nby);
    timer.stop();

    /* count elements in second array from GPU */
    timer.start("CPU: Count-Y_H  ");
    count(y_h, n, t, nbyh);
    timer.stop();

    /* find max abs diff b/t y and y_h */
    diff_y_yh = 0.f;
    int n2 = n+2;
    for (int i=1; i < n; ++i)
        for (int j=1; j < n; ++j)
        {
            float diff = fabsf(y[i*n2 + j] - y_h[i*n2 + j]);
            diff_y_yh = diff > diff_y_yh ? diff : diff_y_yh;
        }

    std::cout << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "-------" << std::endl;
    std::cout << "Number of elements in a row/column         :: " << n+2 << std::endl;
    std::cout << "Number of inner elements in a row/column   :: " << n << std::endl;
    std::cout << "Total number of elements                   :: " << (n+2)*(n+2) << std::endl;
    std::cout << "Total number of inner elements             :: " << n*n << std::endl;
    std::cout << "Memory (GB) used per array                 :: " << (n+2)*(n+2)*sizeof(float) / (float)(1024*1024*1024) << std::endl;;
    std::cout << "Threshold                                  :: " << t << std::endl;
    std::cout << "Smoothing constants (a, b, c)              :: " << a << " " << b << " " << c << std::endl;
    std::cout << "Number   of elements below threshold (X)   :: " << nbx << std::endl;
    std::cout << "Fraction of elements below threshold       :: " << nbx / (float)(n*n) << std::endl;
    std::cout << "Number   of elements below threshold (Y)   :: " << nby << std::endl;
    std::cout << "Fraction of elements below threshold       :: " << nby / (float)(n*n) << std::endl;
    std::cout << "Number   of elements below threshold (Y_H) :: " << nbyh << std::endl;
    std::cout << "Fraction of elements below threshold       :: " << nbyh / (float)(n*n) << std::endl;
    std::cout << "Maximum abs. difference between y and y_h  :: " << diff_y_yh << std::endl;
    std::cout << "Block Size                                 :: " << blockSize.x << " " << blockSize.y << " " << blockSize.z << std::endl;
    std::cout << "Grid Size                                  :: " << gridSize.x << " " << gridSize.y << " " << gridSize.z << std::endl;
    std::cout << "Number of threads per block                :: " << blockSize.x * blockSize.y << std::endl;
    std::cout << "Number of blocks per grid                  :: " << gridSize.x * gridSize.y << std::endl;
    std::cout << "Total number of threads                    :: " << blockSize.x * blockSize.y * gridSize.x * gridSize.y << std::endl;

    timer.print();

    delete[] x;
    delete[] y;                                 
    delete[] y_h;
    cudaFree(x_d);
    cudaFree(y_d);
}
