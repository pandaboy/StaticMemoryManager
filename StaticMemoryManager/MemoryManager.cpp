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

  // returns a generic pointer to the byte at buffer[i]
  void* atIndex(unsigned int i = 0)
  {
    // if we try to access beyond the size of the array,
    // return nullptr
    if (i >= MM_POOL_SIZE)
    {
      return nullptr;
    }

    // return pointer using pointer arithmetic
    return ((char *)MM_pool + i);
  }

  // Recursively searches for a free chunk, returns address of free chunk or nullptr
  // size: size of chunk requested
  // i: index of pool to look at
  void* free(int size, unsigned short i = 0)
  {
    void* resource = nullptr;

    // our helpfule struct definition
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // get the header at index i
    header* hp = (header *)atIndex(i);

    if (hp != nullptr)
    {
      // if it's too small, or in use, check the next block
      if (hp->size < size || hp->free != 0)
      {
        // calculate next block by available values:
        // e.g. 0 + 4(long) + 4(header) -> index 8
        resource = free(size, i + hp->size + sizeof(header));
      }
      else
      {
        // if the current block is adequate, point the resource to its address
        // will move sizeof(header) bytes based on pointer arithmetic
        resource = hp + 1;
      }
    }

    return resource;
  }

  // recursively merges adjacent free blocks
  void merge(unsigned short i = 0)
  {
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // left header/chunk
    header* left = (header *)atIndex(i);

    // calculate right header/chunk index position
    // e.g. 0 + 4(long) + 4(header) = 8
    unsigned int next = i + left->size + sizeof(header);

    // right header/chunk
    header* right = (header *)atIndex(next);

    // if either headers are unavailable, nothing to test
    if (left == nullptr || right == nullptr)
    {
      return;
    }

    // check if this chunk and the one next to it are
    // both empty - if so merge them
    if (left->free == 0 && right->free == 0)
    {
      // merging involves adding the right chunk size (header + data)
      // to the current chunk
      left->size += right->size + sizeof(header);

      // call merge on this chunk again.
      // - this is to cover situations where:
      // [Free] -> [Free] -> [Free]
      merge(i);
    }
    else
    {
      // otherwise carry on to the next header
      merge(next);
    }
  }

  // Initialize set up any data needed to manage the memory pool
  void initializeMemoryManager(void)
  {
    // TODO : IMPLEMENT ME

    // struct for tracking allocations:
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // + 65532 | 0 |                                      +
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // We put one at the beginning of each "chunk" of memory that we allocate, it 
    // stores the size and an indicator of whether or not the allocation is in use.
    // Using this struct makes it easier to query the chunks, so I define it at the
    // start of any methods that need it.
    // I could improve on this (and save about 3bytes on each allocation) by using a short 
    // for size and a bool for the 'free' parameter outside of the struct (due to issues
    // with padding in structs), but I won't have the ease of dereferencing with a 
    // pointer to a header*

    // NOTE:
    // I use an unsigned short for header.size because of MM_POOL_SIZE,
    // since the maximum value I can store in a short is 65,535, and at least 4 bytes
    // will always be in use for the first header, I only store upto 65,531 in header.size
    // I could use a bool for free, but I'd just waste 3bytes due to padding so I left this
    // as a short. I had originally had header.free as an index to the next chunk
    // of memory (as header.index) but now I just use it to indicate free(0) or in use(1)
    struct header {
      unsigned short size; // Size of space AFTER header.
      unsigned short free; // Indicates if the chunk is in use.
    };

    // Creates an initial chunk, by setting the size to (MM_POOL_SIZE - header size)
    header* hp = (header *)MM_pool;
    hp->size = MM_POOL_SIZE - sizeof(header);
    hp->free = 0; // 0 indicates a free chunk of space
  }

  // return a pointer inside the memory pool
  void* allocate(int aSize)
  {
    // TODO: IMPLEMENT ME

    // are we trying to assign the impossible?
    if (aSize < 1 || aSize > MM_POOL_SIZE) {
      onIllegalOperation("Invalid size request: %d", 1);
    }
    
    // struct again!
    struct header
    {
      unsigned short size;
      unsigned short free;
    };

    // Recursive search for free block
    void* resource = free(aSize);

    // if we got a resource allocation:
    // - update the header to mark it as in use
    // - check if we can split the resource
    if (resource != nullptr)
    {
      // the header is before the resource by sizeof(header) bytes
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
        header* nhp = (header *)((char *)hp + toAllocate);

        // next we set the information for the new header:
        // size = original size - size_allocated
        nhp->size = hp->size - toAllocate;
        nhp->free = 0;

        // update the original header information
        // with the size allocated
        hp->size = aSize;
      }
    }
    else if(resource == nullptr)
    {
      // if we got a nullptr, there wasn't any space left
      onOutOfMemory();
    }

    return resource;
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
    // our available space always starts sizeof(header) beyond the 
    // start i.e. at 4bytes or MM_Pool[4]
    if (aPointer < atIndex(sizeof(header)) || aPointer > atIndex(MM_POOL_SIZE - 1))
    {
      onIllegalOperation("Invalid reference provided: %d", 2);
    }

    // get the header (always sizeof(header) before the resource)
    header* hp = (header*)aPointer - 1;

    // just mark it as free
    hp->free = 0;

    // next we call merge on the memory to merge any free areas
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
    unsigned int allocated = 0; // stores how much space we've taken up

    while (i < MM_POOL_SIZE) {
      hp = (header *)atIndex(i);

      // add the size of the allocated header
      allocated += sizeof(header);

      // if it's in use, add the size as well
      if (hp->free == 1)
      {
        allocated += hp->size;
      }

      // jump to next header
      i += hp->size + sizeof(header);
    }

    // return how much space we've taken deducted from total size
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
    unsigned int largest = 0; // used to track the biggest allocation

    while (i < MM_POOL_SIZE) {
      hp = (header *)atIndex(i);

      // if it's in use, and bigger than what we have, update our largest
      if (hp->free == 0 && hp->size >= largest)
      {
        largest = hp->size;
      }

      // jump to next header
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
    unsigned int smallest = MM_POOL_SIZE; // start at the maximum size

    while (i < MM_POOL_SIZE) {
      hp = (header *)atIndex(i);

      // if the size is smaller (and in use) update our value
      if (hp->free == 0 && hp->size < smallest)
      {
        smallest = hp->size;
      }

      // jump to next header
      i += hp->size + sizeof(header);
    }

    // If we never updated the pool_size (and it's largest is always
    // MM_POOL_SIZE - 4) then set smalles to 0
    if (smallest == MM_POOL_SIZE)
    {
      smallest = 0;
    }

    return smallest;
  }

 }