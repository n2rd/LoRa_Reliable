#ifndef P2P_H
#define P2P_H

void p2pDumpStats(Print& printDev);
void p2pClearStats();
void p2pDumpCompactStats(Print& printDev);
void p2pStop();
void p2pSetup(bool broadcastOnlyArg = false);
void p2pLoop(void);
void broadcastOnlyLoop(void);
void debugMessage(char* message);

#endif //P2P_H