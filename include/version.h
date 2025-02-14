#ifndef VERSION_H
#define VERSION_H
//version
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
//#define VERSION_DATE "12:20 03-4-2025" //Old method
#define VERSION_DATE BUILD_DATE
#define VERSION_TIME BUILD_TIME
// For now make this the same as VERSION_DATE
#define VERSION VERSION_DATE "\n" VERSION_TIME //Should be a dotted number major.minor.build
#endif