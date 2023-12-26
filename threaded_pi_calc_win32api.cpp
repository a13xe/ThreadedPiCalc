#include <windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <atomic>

// Block size for distribution
const int blockSize = 1308080;
// Total iterations
const int N = 100000000;
const int numBlocks = N / blockSize;

CRITICAL_SECTION cs;
std::atomic<int> nextBlock(0);
double pi = 0.0;
std::vector<bool> processedBlocks(numBlocks, false);

DWORD WINAPI CalculatePiBlock(LPVOID param) 
{
    while (true) 
    {
        int blockIndex = nextBlock.fetch_add(1);

        if (blockIndex >= numBlocks)
        {
            break;
        }

        double blockSum = 0.0;
        int start = blockIndex * blockSize;
        int end = start + blockSize;
        for (int i = start; i < end; ++i) 
        {
            double x = (i + 0.5) / N;
            blockSum += 4.0 / (1.0 + x * x);
        }

        EnterCriticalSection(&cs);
        pi += blockSum;
        LeaveCriticalSection(&cs);
    }
    return 0;
}

void runCalculation(int numThreads) 
{
    InitializeCriticalSection(&cs);
    std::vector<HANDLE> threads(numThreads);
    std::vector<int> threadNums(numThreads);

    for (int i = 0; i < numThreads; ++i) 
    {
        threadNums[i] = i;
        threads[i] = CreateThread(NULL, 0, CalculatePiBlock, &threadNums[i], 0, NULL);
    }

    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    for (HANDLE thread : threads) 
    {
        CloseHandle(thread);
    }

    DeleteCriticalSection(&cs);
}

int main() 
{
    std::vector<int> threadCounts = {1, 2, 4, 8, 12, 16, 32, 64};
    std::ofstream resultsFile("threaded_pi_calc_win32api_results.txt");
    resultsFile << "Threads, Time taken (s)" << std::endl;

    for (int numThreads : threadCounts) 
    {
        pi = 0.0;
        nextBlock.store(0);
        processedBlocks.assign(numBlocks, false);

        // Start timing
        std::cout << "Starting calculation with " << numThreads << " threads...\n";
        auto startTime = std::chrono::high_resolution_clock::now();

        // Perform the computation
        runCalculation(numThreads);

        // End timing
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;

        std::cout << "Threads: " << numThreads << ", Time taken: " << duration.count() << " s, Calculated Pi: " << pi << "\n";
        resultsFile << numThreads << ", " << duration.count() << std::endl;
    }

    resultsFile.close();
    return 0;
}
