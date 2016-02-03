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
  char MM_pool[MM_POOL_SIZE];

  // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {
    // TODO : IMPLEMENT ME
 
    // Initialize buffer chunks to -1 (0xFFFFFFFFFFFFFFFF),
    // can't use memset for this based on the requirements for no headers,
    // so I will simply loop through the pool and initialize the values to -1(0xFFFFFFFFFFFFFFFF),
    // which I use to indicate a free chunk of memory
    for (unsigned i = 0; i < MM_POOL_SIZE; i++) {
      MM_pool[i] = -1;
    }
  }

  // return a pointer inside the memory pool
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME

    // anything less than 1 is invalid
    if (aSize < 1) {
      onIllegalOperation("Invalid size request: %d", 1);
    }

    // Looks for space in the pool, returns a nullptr if no space was found
    void* startPtr = findSpace(aSize);

    // if we can't find space in the pool, complain
    if (startPtr == nullptr) {
      onOutOfMemory();
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

    // if aPointer is null, report error
    if (aPointer == nullptr) {
      onIllegalOperation("Invalid reference provided: %d", 2);
    }

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
    unsigned bytes = 0; // number of 'free' bytes

    // search through all the items looking at values
    for (unsigned i = 0; i < MM_POOL_SIZE; i++) {
      if (MM_pool[i] == -1) {
        bytes++;
      }
    }

    return bytes;
  }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
    // TODO: IMPLEMENT ME
    unsigned largest = 0;          // starts at Minimum value and adds up
    unsigned bytes = 0; // number of bytes we've counted in a row

    for (unsigned i = 0; i < MM_POOL_SIZE; i++) {
      // check if 'free', if it is we'll update the bytes
      if (MM_pool[i] == -1) {
        // add to the count
        bytes++;
      }
      // if not 'free', reset the count
      else {
        bytes = 0;
      }

      // if what we have is larger than our current total, top up
      if (bytes > largest) {
        largest = bytes;
      }
    }

    return largest;
  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
    // TODO: IMPLEMENT ME

    unsigned smallest = MM_POOL_SIZE; // start at Maximum pool size, work our way down.
    unsigned bytes = 0;    // number of bytes

    // check each byte
    for (unsigned i = 0; i < MM_POOL_SIZE; i++) {
      // check if marked as 'free' && part of a 'free' sequence
      if (MM_pool[i] == -1) {
        bytes++;
      }
      // if not free, reset the number of bytes to 0
      else {
        // if the number of bytes we have are smaller than
        // the current smallest value update it
        if (bytes < smallest && bytes != 0) {
          smallest = bytes;
        }

        bytes = 0;
      }
    }

    // do a final check
    if (bytes < smallest) {
      smallest = bytes;
    }

    return smallest;
  }

  // returns address of array to start allocating memory to
  void* findSpace(unsigned aSize)
  {
    char* byte = MM_pool;   // initialize to the beginning of the buffer, and to 1 byte
    void* start = nullptr;  // this is our return value
    unsigned bytes = 0;          // number of bytes we've counted
    int prev = 0;           // holds our previous byte value

    // keep going unless we hit a terminating character, 
    // or attempt to go beyond the last item in the array
    while (bytes != aSize && byte != &MM_pool[MM_POOL_SIZE]) {
      int byteInt = static_cast<int>(*byte);

      if (byteInt == -1) {
        // count only free chunks
        bytes++;
      }

      if (byteInt != prev) {
        start = byte;
      }

      // move to the next chunk
      byte++;
      prev = byteInt;
    }

    // if we didn't find enough bytes, return nullptr
    if (bytes < aSize) {
      return nullptr;
    }

    // otherwise return our starting point in the array
    return start;
  }

 }