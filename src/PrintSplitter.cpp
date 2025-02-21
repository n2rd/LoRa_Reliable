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

    PrintSplitter::PrintSplitter(Print &_a, Print &_b) : a(_a), b(_b), c(_b) { _cnt = 2; }
    PrintSplitter::PrintSplitter(Print &_a, Print &_b, Print &_c) : a(_a), b(_b), c(_c) { _cnt = 3; }

    size_t PrintSplitter::write(uint8_t ch) {
      a.write(ch);
      size_t retval =  b.write(ch);
      if (_cnt == 3)
        c.write(ch);
      return retval;
    }
    size_t PrintSplitter::write(const char* str) {
      a.write(str);
      size_t retval = b.write(str);
      if (_cnt == 3)
        c.write(str);
      return retval;
    }


