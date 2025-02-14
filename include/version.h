#ifndef VERSION_H
#define VERSION_H
//version
#define BUILD_DATE __DATE__
//#define VERSION_DATE "12:20 03-4-2025" //Old method
#define VERSION_DATE BUILD_DATE
// For now make this the same as VERSION_DATE
#define VERSION VERSION_DATE //Should be a dotted number major.minor.build
#endif