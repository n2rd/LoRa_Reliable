#ifndef PrintSplitter_h
#define PrintSplitter_h

class PrintSplitter : public Print {
  public:
    PrintSplitter(Print &_a, Print &_b);
    PrintSplitter(Print &_a, Print &_b, Print &_c);
    size_t write(uint8_t c);
    size_t write(const char* str);
  private:
    int _cnt;
    Print &a;
    Print &b;
    Print &c;    
};

#endif