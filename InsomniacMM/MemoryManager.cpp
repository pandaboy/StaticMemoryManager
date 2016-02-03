#include "MemoryManager.h"

namespace MemoryManager
{
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
  //
  // This is the only static memory that you may use, no other global variables may be
  // created, if you need to save data make it fit in MM_pool
  //
  // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT


  const int MM_POOL_SIZE = 65536;
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
  // If no chunk can accommodate aSize call onOutOfMemory()
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME
    int start_at = findSpace(aSize); // byte in buffer to start at - source from search function

    // if we can't find space, shout that we're out of memory (for that allocation)
    if (start_at == -1) {
      onIllegalOperation("Not enough space!\n- Total Available: %d bytes\n- Requested: %d bytes\nLargest Block: %d\n", freeRemaining(), aSize, largestFree());
    }

    // allocate bytes by setting them to 0x00 from start_at
    int i = start_at;
    int block = aSize * 8;
    for (i = start_at; i < (aSize + start_at); i++) {
      // initialize to null character
      MM_pool[i] = 0x00;
    }

    // using this value as a terminating value for the buffer
    // [0x00, 0x00, 0x00, 0x00, -2] <-- indicates end of buffer zone
    // since I'm not tracking the size of the buffers allocated.
    MM_pool[i] = -2;

    // return the address of the allocated buffer
    return &MM_pool[start_at];
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

  int findSpace(int aSize)
  {
    // look for free space in the buffer, return index
    int count = 0; // counts consecutive free buffers
    int idx = 0; // indicates the index in the array
    int prev = 0; // tracks the previous byte we just checked
    // used in loop below
    int iMM = 0, i = 0;

    for (i = 0; i < MM_POOL_SIZE; i++)
    {
      iMM = static_cast<int>(MM_pool[i]);

      // update count if we are on consecutive bytes
      if (iMM == -1 && prev == -1) {
        count++;
      }
      else {
        idx = i;
      }

      if (count == aSize) {
        return idx;
      }

      prev = iMM;
    }

    // if we find nothing, return -1
    return -1;
  }

 }