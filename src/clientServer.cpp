#include "main.h"

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
static int Failure_Counter = 0;
static int debugLoopCount = 0;
static RHGenericDriver::RHMode lastMode = (RHGenericDriver::RHMode)-1;
// temporary transmit buffer
static uint8_t data[20];
// Dont put this on the stack: 
// it is fragile, you will break it if you touch it
// do not rename, etc.,  if you mess with it, you won't get anything that is received
static uint8_t buf[DRIVER_MAX_MESSAGE_LEN];
//message management
static uint16_t counter = 0;
static uint64_t tx_time = 0;

void clientServerLoop()
{
  //now operate in different roles
  if (PARMS.parameters.address == 1)  //serving as a server
  {
    driver.setModeRx();
    if (driver.mode() != lastMode) {
      Serial.printf("driver.mode changed to %d\n",driver.mode());
      lastMode = driver.mode();
      DisplayUpperRight(lastMode);
    }
    else {
      //Serial.println("Driver mode not changed");
    }

    if (manager.available())  //message has come in
    {
      Serial.printf("Got a Message. Server Count= %d\n",debugLoopCount++);
      // Wait for a message addressed to us from the client
      uint8_t len = sizeof(buf);
      uint8_t from = 0;
      uint8_t to = 0;
      uint8_t id = 0;
      uint8_t flags = 0xFF;
      if (manager.recvfromAck(buf, &len, &from, &to, &id, &flags))
      {
        int snr = driver.lastSNR();
        int rssi = driver.lastRssi();
        display.printf("%i -> %i\n", from, (int)(buf[1]*256 + buf[0]));
        display.printf("RSSI %i   SNR %i flags: %i\n", rssi, snr, flags);
        rssi = abs(rssi);
        data[0] = static_cast<uint8_t>(rssi);
        data[1] = static_cast<uint8_t>(snr);
        Serial.printf("%i, %i, %i, -%i, %i, %d\n", millis(), from, (int)(buf[1]*256 + buf[0]), rssi, snr, manager.retransmissions());
        manager.resetRetransmissions();
        if (!manager.sendtoWait(data, 2, from)) {
          //display.println("sendtoWait failed");
          Serial.printf("sendtoWait to %d failed in %s %d\n", from,__FILE__,__LINE__);
          DisplayUpperRight(++Failure_Counter);
        }
        else
          DisplayUpperRight(Failure_Counter);
      }
      else {
        //We can get here because there was no packet received, it wasn't for us or was an ACK
        Serial.printf("manager.recvfromAck FAILED flags= %X at Line %d in %s\n", flags,__LINE__,__FILE__);
      }
    }
  }  //address 1 SERVER

  if (PARMS.parameters.address > 1)  //serving as a client
  {  
    // Send a message to manager_server
    if ((millis() - tx_time) > (PAUSE * 1000)) 
    {
      tx_time = millis();
      data[0] = static_cast<uint8_t>(counter & 0xFF); //low byte
      data[1] = static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
      manager.resetRetransmissions();
      Serial.printf("before manager.sendtowait line %d in %s\n",__LINE__,__FILE__);
      if (manager.sendtoWait((uint8_t *)data, 2, 1))
      {
        int retransmisison_count = manager.retransmissions();
        display.print("Sent ");
        display.print((int)(data[1]*256 + data[0]));
        display.printf(" retrans = %i\n", retransmisison_count);

        // Now wait for a reply from the server
        uint8_t len = sizeof(buf);
        uint8_t from;   
        if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
        {
          display.printf("1 -> RSSI -%i SNR %i\n", (int)buf[0], (int)buf[1]);
          int snr = driver.lastSNR();
          int rssi = driver.lastRssi();
          display.printf("%i <- RSSI %i SNR %i\n", PARMS.parameters.address, rssi, snr);
          Serial.printf("%i, %i, -%i, %i, %i, %i\n", millis(), counter, (int)buf[0], (int)buf[1], rssi, snr);
        } else {
          display.println("No return reply");
        }
      } else {
        int retransmisison_count = manager.retransmissions();
        display.printf("%s sendtoWait failed %i retries\n", data, retransmisison_count);
        Serial.printf("%i, %i, sendtoWait Failed %i retries\n", millis(), counter,retransmisison_count);
        //DisplayUpperRight(Failure_Counter++);
      }
      DisplayUpperRight(counter);
      counter++;
    } //legal to transmit
  } // as a client
} //ClientServeLoop