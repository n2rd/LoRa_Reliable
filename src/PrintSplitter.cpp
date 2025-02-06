#include <Arduino.h>
#include "PrintSplitter.h"
/**
 * @class PrintSplitter
 * @brief A class that splits the output of the Print class to two different
 *        Print objects.
 *
 * The PrintSplitter class is used to split the output of the Print class to two
 * different Print objects. It overrides the write() function to write the data
 * to both Print objects.
 */

    PrintSplitter::PrintSplitter(Print &_a, Print &_b) : a(_a), b(_b) {}

    size_t PrintSplitter::write(uint8_t c) {
      a.write(c);
      return b.write(c);
    }
    size_t PrintSplitter::write(const char* str) {
      a.write(str);
      return b.write(str);
    }


