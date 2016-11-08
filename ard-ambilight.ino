// Ethernet stuff
#include <SPI.h> // needed for Arduino versions later than 0018
#include <Ethernet.h>

#define UDP_TX_PACKET_MAX_SIZE 90 // Increase UDP packet size to fit our payload (default is 24)

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

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
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
}

void loop() {
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
    
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      // read the packet into packetBufffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

      // Payload example:
      // /0:R070G045B028/1:R068G046B031/2:R066G044B029/3:R064G039B030/4:R064G040B028/5:R070G048B031

      processArea(packetBuffer, 0);
      processArea(packetBuffer, 1);
      processArea(packetBuffer, 2);
      // processArea(packetBuffer, 3); // we don't have LEDs for that part of the screen (yet)
      processArea(packetBuffer, 4);
      processArea(packetBuffer, 5);

      FastLED.show();
    }
    delay(10);
}

void processArea(char *packetBuffer, int areaNum) {
    unsigned int charPos = areaNum * 15;

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
    unsigned int ledStartPos;
    unsigned int ledCnt;

    switch (areaNum) {
    case 0:
        ledStartPos = 11;
        ledCnt = 17;
        break;
    case 1:
        ledStartPos = 1; // or 0
        ledCnt = 9; // or 10
        break;
    case 2:
        ledStartPos = 28;
        ledCnt = 15;
        break;
    case 4:
        ledStartPos = 43;
        ledCnt = 17;
        break;
    case 5:
        ledStartPos = 61;
        ledCnt = 9;
        break;
    default:
        // Nothing to do
        return;
    }

    charPos += 4; // "/5:R"
    int red = getColorFromMessage(packetBuffer, charPos);
    charPos += 4; // "255G"
    int green = getColorFromMessage(packetBuffer, charPos);
    charPos += 4; // "255B"
    int blue = getColorFromMessage(packetBuffer, charPos);

    CRGB rgb = CRGB(red, green, blue);
    for(unsigned int i=0; i<ledCnt; i++){
        leds[ledStartPos + i] = rgb;
    }
}

int getColorFromMessage(char *packetBuffer, int charPos) {
    char buffer[4];
    buffer[0] = packetBuffer[charPos];
    buffer[1] = packetBuffer[charPos + 1];
    buffer[2] = packetBuffer[charPos + 2];
    buffer[3] = '\0';

    return atoi(buffer);
}


