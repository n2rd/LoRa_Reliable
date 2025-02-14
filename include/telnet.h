#ifndef TELNET_H
#define TELNET_H

#include "ESPTelnet.h"

extern ESPTelnet telnet;

void telnet_setup();
void telnet_loop();

#endif //TELNET_H