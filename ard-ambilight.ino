// Ethernet stuff
#include <SPI.h> // needed for Arduino versions later than 0018
#include <Ethernet.h>

#define UDP_TX_PACKET_MAX_SIZE 90 // Increase UDP size to fit our payload (default is 24)

#include <EthernetUdp.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008

// LED stuff
#include "FastLED.h"
#define NUM_LEDS 69
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

// MAC address of ethernet board
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
    //== LED setup
    // Sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);

    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.show();

    //== Ethernet setup
    // Start the Ethernet and UDP:
    Ethernet.begin(mac, ip);
    Udp.begin(localPort);

    // Send start message to server
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

      // Payload example:
      // /0:R070G045B028/1:R068G046B031/2:R066G044B029/3:R064G039B030/4:R064G040B028/5:R070G048B031

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
  unsigned int charPos = areaNum * 15;
  //unsigned int ledPos = areaNum * 15;

/*

       LEDs |   8        15         8
      ----- -----------------------------
            |       |           |       |
        9   |   0   |     2     |   4   |   50%
            |       |           |       |
      ----- -----------------------------
            |       |           |       |
        9   |   1   |     3     |   5   |   50%
      (+1)  |       |           |       |
      ----- -----------------------------
            |  20%  |    40%    |  20%  | Ratio
*/
  Serial.print("Area #");
  Serial.print(packetBuffer[charPos + 1]);
  Serial.println(":");
  
  Serial.println("Red:");
  Serial.print(packetBuffer[charPos + 4]);
  Serial.print(packetBuffer[charPos + 5]);
  Serial.println(packetBuffer[charPos + 6]);
}


