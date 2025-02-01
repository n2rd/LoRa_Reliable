# LoRa_Reliable
LoRa point to point communication between server and multiple clients using RadioHead Reliable Datagram

**Addresses**
Server address 1:  All clients, with addresses > 1 communicate with the server
Client addresses 2 and 3: communicate with the server
Set the address on line 17 by changing the value of MY_ADDRESS

**Communication:**
  A client sends a consecutive counter number using reliable datagram to the server.
  If the server received it, it acks and then sends the RSSI and the SNR of the received signal back to the client
  The return communication to the client is sent using a datagram but not using the reliable protocol

**The USER button (the top one) is programmed:**  
   A _single-click_ shows the power setting.  
   Additional single-click within 2s changes the power setting the next value and sets the radio accordingly.  
   The available power settings are -9, -5, 0, 6, 12, 18, and 22 dBm.

   A _double-click_ shows the modulation setting.  
   Additonal double-click within 2s changes the modulation setting to the next value and sets the radio accordingly.  
   The modulation settings are:

LoRa settings that are used for Meshtastic

| No | Channel Name  | Data-Rate	| SF  | Symb	| Coding Rate	| Bandwidth	| Link Budget |
|----|---------------|------------|-----|-------|-------------|-----------|-------------|
| 0  |Short Turbo   | 21.88 kbps | 7   | 128   |	   4/5	    |   500 kHz	|  140dB      |
| 1  |Short Fast    | 10.94 kbps | 7 | 128	 | 4/5	        | 250 kHz	  | 143dB |
| 2  |Short Slow	  | 6.25 kbps	| 8 | 256	  | 4/5	        | 250 kHz	  | 145.5dB |
| 3  |Medium Fast	   | 3.52 kbps	| 9 | 512	  | 4/5	        | 250 kHz	  | 148dB |
| 4  |Medium Slow	   | 1.95 kbps	| 10 | 1024	| 4/5	        | 250 kHz	  | 150.5dB |
| 5  |Long Fast	     | 1.07 kbps	| 11 | 2048	| 4/5	        | 250 kHz	  | 153dB |
| 6  |Long Moderate  | 0.34 kbps	| 11 | 2048	| 4/8	        | 125 kHz	  | 156dB |
| 7  |Long Slow	     | 0.18 kbps	| 12 | 4096	| 4/8	        | 125 kHz	  | 158.5dB |
| 8  |Very Long Slow | 0.09 kbps	| 12 | 4096 |	4/8	        | 62.5 kHz	| 161.5dB |

  A _long press_ (more than 1 sec long) puts the units into deep sleep, making it safe to change the antenna
  
**Logging**

Sent to serial port, in CSV format
```
/***  logging format ***
*  server
*     millis, from, counter, rssi, snr, send_report_back_status
* 
*  client
*   successful
*     millis, counter, rssi, snr, rssi_reported_by_server, snr_report 
*   failed
*     millis, counter, "failed"
*/
'''
