#ifndef P2P_H
#define P2P_H

extern bool tx_lock;
extern bool short_pause;

void p2pSetup(void);
void p2pLoop(void);

#endif //P2P_H