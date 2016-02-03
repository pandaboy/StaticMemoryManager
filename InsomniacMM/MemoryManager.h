#pragma once

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
//
// DO NOT CHANGE THIS HEADER FILE
//
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 


namespace MemoryManager
{
  //---
  //--- CORE Functions, these will need to be completed by the applicant
  //---

  // Initialize any data needed to manage the memory pool
  void initializeMemoryManager(void);

  // return a pointer inside the memory pool
  // If no chunk can accommodate aSize call OnAllocFail()
  void* allocate(int aSize);

  // Free up a chunk previously allocated
  void deallocate(void* aPointer);

  //---
  //--- support routines
  //--- 

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void);

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void);

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void);

  //---
  //--- error conditions. None of these functions will return
  //--- These routines do not need to be implemented by the candidate
  //---

  // Call if no space is left for the allocation request
  void onOutOfMemory(void);

  // If the caller makes any illegal request your code should call this 
  // provided failure function (which will not return):
  void onIllegalOperation(const char* fmt,...);
  // eg:
  //   int errorCode;
  //   ...
  //   onIllegalOperation("Error in createQueue: %d", errorCode);

  // finds available space in the buffer and returns the index to start from, returns -1 if none found
  int findSpace(int aSize);
};


#endif  // __MEMORY_MANAGER_H__
