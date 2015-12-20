// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#include <DigiFi.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      420

#define SERVER_PORT    8080

#define BUFSIZE 255

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
DigiFi server;
bool pixelsChanged = true;

void setup() {
  setupPixels();
  startServer();
}

void setupPixels() {
  pixels.begin(); // This initializes the NeoPixel library.
}

void startServer() {
  // start the server:
  server.begin();
  server.server(SERVER_PORT);
  while (server.ready() != 1) {
    delay(100);
  }
}

void loop() {
  // note: both the http library and neopixels library use and/or block
  // interrupts. We can't be doing one while doing the other
  if (server.serverRequest()) {
    handleServerLoop();
  } else if(pixelsChanged) {
    sendColors();
  }

  delay(1);
}

void handleServerLoop() {
  char request_path[50];
  int low_led = -1;
  int high_led = -1;
  int red = -1;
  int green = -1;
  int blue = -1;

  server.serverRequestPath().toCharArray(request_path, 50);

  sscanf(request_path, "/?command=%d,%d,%d,%d,%d", &low_led, &high_led, &red, &green, &blue);
  if (low_led >= 0 && high_led >= 0 && red >= 0 && green >= 0 && blue >= 0) {
    // everything was set, this is a valid request
    if(low_led > NUMPIXELS) low_led = NUMPIXELS;
    if(high_led > NUMPIXELS) high_led = NUMPIXELS;
    for(int led=low_led; led<high_led; led++) {
      setColor(led, red, green, blue);
    }
    server.serverResponse("<html><body><p>Setting some lights!</p></body></html>", 200);
  } else {
    server.serverResponse("<html><body><p>No lights set this time. :(</p></body></html>", 200);
    delay(10);
  }
}

void setColor(int led, int red, int green, int blue) {
  red   &= 0xFF;
  green &= 0xFF;
  blue  &= 0xFF;

  pixels.setPixelColor(led, pixels.Color(red, green, blue));
  pixelsChanged = true;
}

void sendColors() {
  pixels.show(); // This sends the updated pixel color to the hardware.
  pixelsChanged = false;
}
