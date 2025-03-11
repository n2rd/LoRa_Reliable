#ifndef P2P_H
#define P2P_H

extern bool tx_lock;
extern bool short_pause;

void p2pSetup(bool broadcastOnlyArg = false);
void p2pLoop(void);
void broadcastOnlyLoop(void);

#endif //P2P_H