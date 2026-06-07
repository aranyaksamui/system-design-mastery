/*
    Stage 3: Simulating the virtual memory with absolute basic demonstration that shows 
    how the OS handles memory like the stack, the heap, and clean up operations.
*/

#include <iostream>
#include <cstring>

// A function to simulate OS memory operation when a network request comes 
void handleRequestProperly(int connectionId, size_t payloadSize)
{
    // Local variable (stack allocation)
    int localConnectionId = connectionId;
    // Local variable (stack allocation) of a pointer to an allocated contagious block (heap memory)
    char* payloadPtr = new char[payloadSize];

    // Writing data on the heap (data is of 10MB) (simulating network operation)
    std::memset(payloadPtr, 'A', payloadSize - 1);
    payloadPtr[payloadSize - 1] = '\0';

    // Log network operation
    std::cout << "Processsed connection: " << localConnectionId << ". Payload size: " << payloadSize << " bytes." << std::endl;

    // Manual clean up of payload pointer after network operation
    delete[] payloadPtr;
}

void handleRequestWithLeak(int connectionId, size_t payloadSize)
{
    // Local variable (stack allocation)
    int localConnectionId = connectionId;
    // Local variable (stack allocation) of a pointer to an allocated contagious block (heap memory)
    char* payloadPtr = new char[payloadSize];

    // Writing data on the heap (data is of 10MB) (simulating network operation)
    std::memset(payloadPtr, 'a', payloadSize - 1);
    payloadPtr[payloadSize - 1] = '\0';

    // Log network operation
    std::cout << "Processed connection: " << localConnectionId << ". Payload size: " << payloadSize << " bytes." << std::endl;

    // No manual cleanup (memory leak)
}

int main()
{
    std::cout << "Starting memory management simulation..." << std::endl;

    // The payload size: 10MB
    size_t payloadSize = 10 * 1024 * 1024;

    // Run simulation to handle request properly
    handleRequestProperly(101, payloadSize);
    // Run simulation to handle request with memory leak
    handleRequestWithLeak(102, payloadSize);

    return 0;
}