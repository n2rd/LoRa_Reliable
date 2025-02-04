#ifndef PrintSplitter_h
#define PrintSplitter_h

class PrintSplitter : public Print {
  public:
    PrintSplitter(Print &_a, Print &_b);
    size_t write(uint8_t c);
    size_t write(const char* str);
  private:
    Print &a;
    Print &b;    
};

#endif