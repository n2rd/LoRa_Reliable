#ifndef P2P_H
#define P2P_H

extern bool tx_lock;
extern bool short_pause;

void dumpStats(Print& printDev);
void p2pStop();
void p2pSetup(bool broadcastOnlyArg = false);
void p2pLoop(void);
void broadcastOnlyLoop(void);
void debugMessage(char* message);

#endif //P2P_H