/*
 UDPSendReceiveString:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

#define UDP_TX_PACKET_MAX_SIZE 90 //increase UDP size (default is 24)

#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0D, 0x0F, 0xB3
};
IPAddress ip(192, 168, 2, 23);
IPAddress server(192, 168, 2, 16);
char startMessage[] = "start";

unsigned int localPort = 64000; // local port to listen on


// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
    // start the Ethernet and UDP:
    Ethernet.begin(mac, ip);
    Udp.begin(localPort);

    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(server, 64001);
    Udp.write(startMessage);
    Udp.endPacket();
    
    Serial.begin(9600);
    Serial.println(UDP_TX_PACKET_MAX_SIZE);
}

void loop() {
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
    
    
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
  
      // read the packet into packetBufffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      Serial.println("Contents:");
      //Serial.println(packetBuffer);

      readArea(packetBuffer, 0);
      readArea(packetBuffer, 1);
      readArea(packetBuffer, 2);
      readArea(packetBuffer, 3);
      readArea(packetBuffer, 4);
      readArea(packetBuffer, 5);

    }
    delay(10);
}

void readArea(char *packetBuffer, int areaNum) {
  Serial.print("Area #");
  Serial.print(packetBuffer[areaNum * 15 + 1]);
  Serial.println(":");
  
  Serial.println("Red:");
  Serial.print(packetBuffer[areaNum * 15 + 4]);
  Serial.print(packetBuffer[areaNum * 15 + 5]);
  Serial.println(packetBuffer[areaNum * 15 + 6]);
}


