#include "MemoryManager.h"
#include <iostream>
using namespace std;

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

    // struct for tracking allocations:
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // + 65532 | 0 |                                      +
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // we put one at the beginning of each "chunk" of memory that we allocate, it 
    // stores the size and an indicator of whether or not the allocation is in use.
    // Using this struct makes it easier to query the chunks, so I define at the
    // start of any methods that need it.
    // I could do this (and save about 3bytes on each allocation) by using a short 
    // for size and a bool for the 'free' parameter, but I won't have the ease of
    // dereferencing with a pointer to a header*
    struct header {
      unsigned short size; // Size of space AFTER header.
      unsigned short free; // Indicates if the chunk is in use.
    };

    // Essentially creates the initial chunk, by setting the
    // size to the MM_POOL_SIZE - the header
    header *hp = (header *)MM_pool;
    hp->size = MM_POOL_SIZE - sizeof(header);
    hp->free = 0; // 0 indicates a free chunk of space
  }

  // Looks for a free chunk, returns address of free chunk
  // size: size of chunk requested
  // i: index of pool to look at
  void* free(int size, unsigned short i)
  {
    // pointer to the allocation,
    void* resource = nullptr;

    // helpful struct here again.
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // get the header at index i
    header *hp = (header *)atIndex(i);

    // if the header is null, we've tried to access beyond the end of the array
    if (hp != nullptr)
    {
      // if it's too small, or in use, check the next block
      if (hp->size < size || hp->free != 0)
      {
        // recursively search for next available chunk
        // returns nullptr if no chunk found
        resource = free(size, i + hp->size + sizeof(header));
      }
      else
      {
        // if the size is adequate and the chunk free,
        // point the resource pointer to the location just after the header
        resource = hp + 1;
      }
    }

    return resource;
  }

  // return a pointer inside the memory pool
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME
    if (aSize < 1) {
      onIllegalOperation("Invalid size request: %d", 1);
    }
    
    // struct again!
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // recursively search for a resource
    void* resource = free(aSize);

    // if we got a resource allocation:
    // - update the header to mark it as in use
    // - check if we can/should split the resource
    // this section will be called multiple times after
    // each recursion call unnecessarily - we
    if (resource != nullptr)
    {
      // make sure the header is up to date.
      header* hp = (header *)resource - 1;

      // mark the chunk as in use
      hp->free = 1;

      // Check if we can split this chunk into smaller chunks
      // - calculate the minimum space we need for a split
      unsigned short toAllocate = aSize + sizeof(header);

      if (toAllocate < hp->size)
      {
        // Create a new header after the resource
        // the index here will point to where the next header should be located at:
        // - resource address + number of bytes to use in split
        header *nhp = (header *)((char *)hp + toAllocate);

        // next we set the information for the new header:
        // size = original size - size_allocated
        nhp->size = hp->size - toAllocate;
        nhp->free = 0;

        // update the original header information
        hp->size = aSize;
      }
    }

    if (resource == nullptr) {
      onOutOfMemory();
    }

    return resource;
  }

  // returns pointer to next free block
  void merge(unsigned short i)
  {
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // left header/chunk
    header* left = (header *)atIndex(i);

    // calculate right header/chunk index position
    unsigned int next = i + left->size + sizeof(header);
    
    // right header/chunk
    header* right = (header *)atIndex(next);

    // stop checking if either headers are null
    if (left == nullptr || right == nullptr)
    {
      return;
    }

    //cout << "i:\t " << i << endl;
    //cout << "next:\t " << next << endl;
    //cout << "----------------------\n";
    
    // check if this chunk and the one next to it are
    // both empty - if so merge them
    if (left->free == 0 && right->free == 0)
    {
      // merging involves adding the next chunk size (header + data)
      // to the current chunk
      left->size += right->size + sizeof(header);

      // call merge on this chunk again.
      // - this is to cover situations where:
      // [Free] -> [Free] -> [Free]
      //merge(i);
    }
    //else
    //{
      merge(next);
    //}
  }

  // Free up a chunk previously allocated
  void deallocate(void* aPointer)
  {
    // TODO: IMPLEMENT ME
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // if aPointer is null, report error
    if (aPointer == nullptr)
    {
      onIllegalOperation("Invalid reference provided: %d", 2);
    }

    // if aPointer is outside the bounds of MM_pool, report error
    // our available space always starts sizeof(header) beyond the start
    if (aPointer < atIndex(sizeof(header)) || aPointer > atIndex(MM_POOL_SIZE - 1))
    {
      onIllegalOperation("Invalid reference provided: %d", 2);
    }

    // get the header (always sizeof(header) before the resource)
    header* hp = (header*)aPointer - 1;

    // just mark it as free
    hp->free = 0;

    // next we call merge on the memory to merge any free areas
    //cout << "Calling merge\n";
    merge();
  }

  //---
  //--- support routines
  //--- 

  // Will scan the memory pool and return the total free space remaining
  int freeRemaining(void)
  {
    // TODO: IMPLEMENT ME
    struct header
    {
      unsigned short size;
      unsigned short free;
    } *hp;

    // loop through the buffer, incrementing based on header information
    unsigned int i = 0;
    unsigned int allocated = 0;

    while (i < MM_POOL_SIZE) {
      // hp = (header *)((char *)MM_pool + i);
      hp = (header *)atIndex(i);

      if (hp->free == 1)
      {
        allocated += hp->size + sizeof(header);
      }
      else
      {
        allocated += sizeof(header);
      }

      // bytes to increment by
      i += hp->size + sizeof(header);
    }

    return MM_POOL_SIZE - allocated;
  }

  // Will scan the memory pool and return the largest free space remaining
  int largestFree(void)
  {
    // TODO: IMPLEMENT ME
    struct header
    {
      unsigned short size;
      unsigned short free;
    } *hp;

    // loop through the buffer, incrementing based on header information
    unsigned int i = 0;
    unsigned int largest = 0;

    while (i < MM_POOL_SIZE) {
      //hp = (header *)((char *)MM_pool + i);
      hp = (header *)atIndex(i);

      if (hp->free == 0 && hp->size >= largest)
      {
        largest = hp->size;
      }

      // bytes to increment by
      i += hp->size + sizeof(header);
    }

    return largest;
  }

  // will scan the memory pool and return the smallest free space remaining
  int smallestFree(void)
  {
    // TODO: IMPLEMENT ME
    struct header
    {
      unsigned short size;
      unsigned short free;
    } *hp;

    // loop through the buffer, incrementing based on header information
    unsigned int i = 0;
    unsigned int smallest = MM_POOL_SIZE;

    while (i < MM_POOL_SIZE) {
      //hp = (header *)((char *)MM_pool + i);
      hp = (header *)atIndex(i);

      if (hp->free == 0 && hp->size < smallest)
      {
        smallest = hp->size;
      }

      // bytes to increment by to get to next chunk
      i += hp->size + sizeof(header);
    }

    // check if we didn't find any free space
    if (smallest == MM_POOL_SIZE)
    {
      smallest = 0;
    }

    return smallest;
  }

  // returns a generic pointer to buffer[i]
  void *atIndex(unsigned int i)
  {
    // if we try to access beyond the size of the array,
    // return nullptr
    if (i >= MM_POOL_SIZE)
    {
      return nullptr;
    }

    // return MM_pool + i;
    return ((char *)MM_pool + i);
  }

 }