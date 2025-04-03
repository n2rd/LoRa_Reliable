#ifndef PrintSplitter_h
#define PrintSplitter_h
class DummyPrintSplitter : public Print {
  public:
    DummyPrintSplitter();
  private:
    size_t write(uint8_t c);
    size_t write(const char* str);
};
extern DummyPrintSplitter dummyPrintSplitter;

class PrintSplitter : public Print {
  public:
    PrintSplitter();
    PrintSplitter(Print &_a);
    PrintSplitter(Print &_a, Print &_b);
    PrintSplitter(Print &_a, Print &_b, Print &_c);
    PrintSplitter(Print &_a, Print &_b, Print &_c, Print &_d);
    size_t write(uint8_t c);
    size_t write(const char* str);
  private:
    int _cnt;
    Print &a;
    Print &b;
    Print &c;
    Print &d;
    //DummyPrintSplitter& dummy();    
};

#endif