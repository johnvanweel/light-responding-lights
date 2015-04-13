#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <XBee.h>
#include <SoftwareSerial.h>


// Define NewSoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
// Yellow wire on Adafruit Pixels
uint8_t dataPin  = 2;    
// Green wire on Adafruit Pixels
uint8_t clockPin = 3;    

SoftwareSerial nss(ssRX, ssTX);

XBee xbee = XBee();

ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);


void setup() { 
  Serial.begin(9600);
  xbee.setSerial(Serial);
  // start soft serial
  nss.begin(9600);

  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
}

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    
    // Received a sample from a remote xBee
    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      // read analog inputs, there could be up to 4
      if (ioSample.isAnalogEnabled(1)) {
        Serial.println(ioSample.getAnialog(1)/4, DEC);

        // xBee values range 0-1024, I need a 0-256 range for the pixels
        int value = 256-(ioSample.getAnalog(1)/4);
        Serial.println(value, DEC);

        int valuePerc = value * 100 / 256;

        Serial.println(valuePerc*256/100, DEC);
        Serial.println(valuePerc*223/100, DEC);
        Serial.println();

        // Set all the pixels to the same color (255, 223,0)
        for (int i=0; i < 21; i++) {
          strip.setPixelColor(i,Color(valuePerc*256/100,valuePerc*223/100,0));
        }

        // Send the data to the strip
        strip.show();
      }

    }    
  } 
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}


