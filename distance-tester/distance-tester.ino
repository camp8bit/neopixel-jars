#define FASTLED_ALLOW_INTERRUPTS 0
#define RH_ASK_ARDUINO_USE_TIMER2

#include <RH_ASK.h>
#include <FastLED.h>
#include <colorutils.h>
#include <SPI.h> // Not actualy used but needed to compile

#define RH_ASK_ARDUINO_USE_TIMER2
#define FASTLED_ALLOW_INTERRUPTS 0

RH_ASK driver(500);

#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    15

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 50

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

void setup() {
  delay(250); // sanity delay
    if (!driver.init())
         Serial.println("init failed");
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); // .setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(9600);
  Serial.println("Started...");

  // supply power to the transmitter
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); 
}

void shutdown(){
  digitalWrite(10, LOW); 
}
 
#define RH_ASK_MAX_MESSAGE_LEN 1

byte intensity = 255;

void loop()
{
  fill_solid(leds, NUM_LEDS, CRGB(intensity,intensity,intensity));
  FastLED.show();
  intensity = (int) max(0, intensity - 16);

  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf); 

  delay(1000 / FRAMES_PER_SECOND);
  
  if (driver.recv(buf, &buflen)){
    intensity = 255;
  }
}


