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

  DummyPrintSplitter dummyPrintSplitter;

    PrintSplitter::PrintSplitter() : a((Print&)dummyPrintSplitter), b((Print&)dummyPrintSplitter), c((Print&)dummyPrintSplitter) , d((Print&)dummyPrintSplitter) {
      /*
      a = (Print&)dummy;
      b = (Print&)dummy;
      c = (Print&)dummy;
      */
      _cnt = 0; 
    }
    PrintSplitter::PrintSplitter(Print &_a) : a(_a), b(_a), c((Print&)dummyPrintSplitter), d((Print&)dummyPrintSplitter) { _cnt = 1; }
    PrintSplitter::PrintSplitter(Print &_a, Print &_b) : a(_a), b(_b), c((Print&)dummyPrintSplitter), d((Print&)dummyPrintSplitter) { _cnt = 2; }
    PrintSplitter::PrintSplitter(Print &_a, Print &_b, Print &_c) : a(_a), b(_b), c(_c), d((Print&)dummyPrintSplitter) { _cnt = 3; }
    PrintSplitter::PrintSplitter(Print &_a, Print &_b, Print &_c, Print &_d) : a(_a), b(_b), c(_c) , d(_d){ _cnt = 4; }

    size_t PrintSplitter::write(uint8_t ch) {
      size_t retval = 0;
      if (_cnt > 0) {
        retval = a.write(ch);
        if (_cnt > 1)
          b.write(ch);
        if (_cnt > 2)
          c.write(ch);
        if (_cnt > 3)
          d.write(ch);
      }
      return retval;
    }
    size_t PrintSplitter::write(const char* str) {
      size_t retval = 0;
      if (_cnt > 0) {
        retval = a.write(str);
        if (_cnt > 1)
          b.write(str);
        if (_cnt > 2)
          c.write(str);
        if (_cnt > 3)
          d.write(str);
      }
      return retval;
    }

    DummyPrintSplitter::DummyPrintSplitter() {}
    size_t DummyPrintSplitter::write(uint8_t c) { return 0; }
    size_t DummyPrintSplitter::write(const char* str) { return 0; }


