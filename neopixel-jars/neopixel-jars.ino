#include <avr/sleep.h>
#include <FastLED.h>
#include <colorutils.h>

#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    15

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

void setup() {
  delay(250); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); // .setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

#define COOLING  70
#define SPARKING 120

static byte heat[NUM_LEDS];

CRGB color;
byte stateMessage;

void fire(int frame){
  FastLED.setBrightness(255);

  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  
  if( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

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

void pulse(int frame, byte hue){
  FastLED.setBrightness(beatsin8(50));
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 128));
}

void rainbow(int frame, byte hue) {
  FastLED.setBrightness(beatsin8(10));
  byte h = millis() / 100;
  fill_solid(leds, NUM_LEDS, CHSV(h, 255, 128));
}

void sweep(int frame, byte hue ){
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));

  int i = frame % NUM_LEDS;
  leds[i] =  CHSV(hue, 255, 128);
}

void sparkles(int frame){
  FastLED.setBrightness(255);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  byte spark = random(NUM_LEDS);
  leds[spark] = CRGB(255,255,255);
}

void gradient(byte frame){
  FastLED.setBrightness(beatsin8(10));
  fill_rainbow(leds, NUM_LEDS, frame * 4);
}

void flash(byte frame){
  FastLED.setBrightness(255);
  if (random(FRAMES_PER_SECOND / 2) == 0) {
    fill_solid(leds, NUM_LEDS, CRGB(255,255,255));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
  }
}

int currentPattern = 0;
int frame = 0;
long lastRender = 0;
int numPatterns = 4;

void render()
{
  frame++;
  
  currentPattern = (unsigned long) millis() / 1000 / 60 / 5;

  byte hue = (long) millis() / 500;
  
  switch (currentPattern % 7) {
    case  0: fire(frame); break;
    case  1: gradient(frame); break;
    case  2: sparkles(frame); break;
    case  3: pulse(frame, hue); break;
    case  4: sweep(frame, hue); break;
    case  5: rainbow(frame, hue); break;
    case  6: flash(frame); break;
  }

  FastLED.show(); // display this frame
}

void(* resetFunc) (void) = 0;

void sleepUntilTomorrow() {
  FastLED.clear();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // delay(1000 * 10); // 10 seconds
  delay(18 * 60 * 60 * 1000); // 18 hours
  sleep_disable();
  resetFunc();
}

void loop()
{
  if (millis() > 6 * 60 * 60 * 1000) {
    // sleepUntilTomorrow();
  } else {
    render();
    delay(1000 / FRAMES_PER_SECOND);
  }
}


