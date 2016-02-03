#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>

#include "MemoryManager.h"

int main(void)
{
  using namespace MemoryManager;

  initializeMemoryManager();

  long* int_pointer;
  char* string_pointer;
  char* char_pointer;

  std::cout << "Free memory = " << freeRemaining() << std::endl;

  // allocations
  int_pointer = (long *)allocate(sizeof(long));
  std::cout << "Free memory (after long allocation) = " << freeRemaining() << std::endl;
  
  string_pointer = (char*)allocate(255);
  std::cout << "Free memory (after string allocation) = " << freeRemaining() << std::endl;
  
  char_pointer = (char*)allocate(sizeof(char));
  std::cout << "Free memory (after char allocation) = " << freeRemaining() << std::endl;

  // assignments
  *int_pointer = 0xDEADBEEF;
  strcpy(string_pointer, "It was the best of times, it was the worst of times");
  *char_pointer = 'b';

  // printing
  std::cout << "Int value: " << *int_pointer << std::endl;
  std::cout << "String value: " << string_pointer << std::endl;
  std::cout << "Char value: " << (int)*char_pointer << std::endl;

  // updates
  std::cout << "Free memory (after assigning values) = " << freeRemaining() << std::endl;
  std::cout << "Largest free memory chunk = " << largestFree() << std::endl;
  std::cout << "Smallest free memory chunk = " << smallestFree() << std::endl;

  // remove allocations
  deallocate(string_pointer);
  std::cout << "Free memory (after string deallocation) = " << freeRemaining() << std::endl;
  deallocate(char_pointer);
  std::cout << "Free memory (after char deallocation) = " << freeRemaining() << std::endl;
  deallocate(int_pointer);
  std::cout << "Free memory (after complete deallocation) = " << freeRemaining() << std::endl;
}

namespace MemoryManager
{
  void onOutOfMemory(void)
  {
    std::cerr << "Memory pool out of memory" << std::endl;
    exit( 1 );
  }

  // call for any other error condition, providing meaningful error messages are appreciated
  void onIllegalOperation(const char* fmt,...)
  {
    if ( fmt == NULL )
    {
      std::cerr << "Unknown illegal operation" << std::endl;
      exit( 1 );
    }
    else
    {
      char	buf[8192];

      va_list argptr;
      va_start (argptr,fmt);
      vsprintf (buf,fmt,argptr);
      va_end (argptr);

      std::cerr << "Illegal operation: \"" << buf << "\"" << std::endl;
      exit( 1 );
    }
  }
}

