#include "MemoryManager.h"
#include <iostream>

namespace MemoryManager
{
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
  //
  // This is the only static memory that you may use, no other global variables may be
  // created, if you need to save data make it fit in MM_pool
  //
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT


  const int MM_POOL_SIZE = 65536;
  //const int MM_POOL_SIZE = 10;
  char MM_pool[MM_POOL_SIZE];

  // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {
    // TODO : IMPLEMENT ME
 
    // Initialize buffer elements to -1
    // I'm making an assumption here that the buffer will not be
    // used to store char's as int's.
    for (int i = 0; i < MM_POOL_SIZE; i++) {
      MM_pool[i] = -1; // initialize to -1
    }
  }

  // return a pointer inside the memory pool
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME

    // will look for space in the buffer, returns a nullptr if no space was found
    void* startPtr = findSpace(aSize);

    // if we can't find space, shout that we're out of memory (for that allocation)
    if (startPtr == nullptr) {
      onIllegalOperation("Not enough space!\n- Total Available: %d bytes\n- Requested: %d bytes\nLargest Block: %d\n", freeRemaining(), aSize, largestFree());
    }

    // prepare the bytes by setting them to 0x00
    for (int i = 0; i < aSize; i++) {
      *((char*)startPtr + i) = 0x00;
    }

    // add a terminating character
    *((char*)startPtr + aSize) = -2;

    return startPtr;
  }

  // Free up a chunk previously allocated
  void deallocate(void* aPointer)
  {
    // TODO: IMPLEMENT ME

    // while the derefernced value is not equal to our terminating value,
    // keep clearing
    while (*(char *)aPointer != -2) {
      // set the value to null
      *(char *)aPointer = -1;

      // move to next memory location
      aPointer = (void *)((char *)aPointer + 1);
    }

    // clear terminating location as well
    *(char *)aPointer = -1;
  }

  //---
  //--- support routines
  //--- 

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void)
  {
    // TODO: IMPLEMENT ME
    int count = 0;

    for (int i = 0; i < MM_POOL_SIZE; i++) {
      if (MM_pool[i] == -1) {
        count++;
      }
    }

    return count;
  }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
    // TODO: IMPLEMENT ME
    int largest = 0, count = 0;

    for (int i = 0; i < MM_POOL_SIZE; i++) {
      // check if free
      if (MM_pool[i] == -1) {
        // add to the count
        count++;
      }
      // if not free, reset the count
      else {
        count = 0;
      }

      if (count > largest) {
        largest = count;
      }
    }

    return largest;
  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
    // TODO: IMPLEMENT ME
    int smallest = MM_POOL_SIZE, count = 0;

    for (int i = 0; i < MM_POOL_SIZE; i++) {
      // check if free
      if (MM_pool[i] == -1) {
        // add to the count
        count++;
      }
      // if not free, reset the count
      else {
        if (count < smallest && count != 0) {
          smallest = count;
        }

        count = 0;
      }
    }

    if (count < smallest) {
      smallest = count;
    }

    return smallest;
  }

  // returns address of array to start allocating memory to
  void* findSpace(int aSize)
  {
    char* chunk = MM_pool; // initialize to the beginning of the buffer, and to 1 byte
    void* start = nullptr;
    int chunks = 0;
    int prev = 0;

    // keep going unless we hit a terminating character, 
    // or attempt to go beyond the last item in the array
    while (chunks != aSize && chunk != &MM_pool[MM_POOL_SIZE]) {
      int chunkInt = static_cast<int>(*chunk);

      //std::cout << "CHUNK: " << chunkInt << std::endl;

      if (chunkInt == -1) {
        // count only free chunks
        chunks++;
      }

      if (chunkInt != prev) {
        start = chunk;
      }

      // move to the next chunk
      chunk += 1;
      prev = chunkInt;
    }

    // if we didn't find enough bytes, return nullptr
    if (chunks < aSize) {
      return nullptr;
    }

    return start;
  }

 }