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
  driver.init();
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); // .setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

void shutdown(){
  digitalWrite(10, LOW); 
}

#define COOLING  55
#define SPARKING 120

static byte heat[NUM_LEDS];

byte stateMessage;

void fire(int frame){
  FastLED.setBrightness(255);

// Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    if( gReverseDirection ) {
      pixelnumber = (NUM_LEDS-1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void flash(int frame, byte hue = 64){
  byte flashModulus = 8;
  FastLED.setBrightness(255);
  if (frame % flashModulus == 0){
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 128));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  }
}

void swipe(int frame, byte hue = 64){
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  leds[frame % NUM_LEDS] = CHSV(hue, 255, 128);  
  leds[(frame+1) % NUM_LEDS] = CHSV(hue, 255, 128);  
}

void sweep(int frame, byte hue = 64){
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));

  for(int i=0;i < frame % NUM_LEDS; i++){
    leds[i] =  CHSV(hue, 255, 128);
  }
}

/* Random sparkles */
void sparkles(int frame){
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  byte spark = random(NUM_LEDS);
  leds[spark] = CRGB(255,255,255);
}

/* Flash a gradient */
//byte gradientModulus = 10;
//void gradientSetup () {
//  gradientModulus = random(16); 
//}
void gradient(byte frame){
  // fill_gradient(leds, 0, CHSV(64,255,128), NUM_LEDS, CHSV(192,255,128));
    fill_rainbow(leds, NUM_LEDS, frame * 4);

  byte i = sin8(frame);
  FastLED.setBrightness(i);

//  if (frame % 2 == 0) {
//  } else {
//    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
//  }
//  for (int x=0;x<NUM_LEDS;x++){
//    leds[x] = leds[(x + frame) % NUM_LEDS];
//  }
}

//typedef void FUNC(int);
//
//FUNC *patterns[] = {
//  fire,
//  gradient,
//  sparkles,
//  flash
//};

int currentPattern = 0;
int frame = 0;
long lastRender = 0;
int numPatterns = 4;

void render()
{
  frame++;
  //currentPattern = (frame * 2 / FRAMES_PER_SECOND) % sizeof(patterns);
  //currentPattern = 2;

  currentPattern = stateMessage;
  
  switch (currentPattern % 12) {
    case  0: fire(frame); break;
    case  1: gradient(frame); break;
    case  2: sparkles(frame); break;
    case  3: flash(frame, 0); break;
    case  4: sweep(frame, 0); break;
    case  5: swipe(frame, 0); break;
    case  6: flash(frame, 64); break;
    case  7: sweep(frame, 64); break;
    case  8: swipe(frame, 64); break;
    case  9: flash(frame, 224); break;
    case  10: sweep(frame, 224); break;
    case  11: swipe(frame, 224); break;
  }

  // gradient(frame);

  FastLED.show(); // display this frame
}

#define RH_ASK_MAX_MESSAGE_LEN 1

void loop()
{
//  if (millis() - lastRender > 1000 / FRAMES_PER_SECOND) {
//    lastRender = millis();
    render();
//  }

  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf); 

  delay(1000 / FRAMES_PER_SECOND);
  // Serial.print(".");
  
  if (driver.recv(buf, &buflen)){
    // int i;

    // Message with a good checksum received, dump it.
    // driver.printBuffer("Got:", buf, buflen);

    //Serial.print("Message: ");
    // Serial.println((char*)buf);
    //Serial.println(buf[0], HEX);
    if (stateMessage != buf[0]){
      stateMessage = buf[0];
      frame = 0;
    }
    
    // currentPattern = buf[0];
  }
}


