#include <FastLED.h>
#include <colorutils.h>

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
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

#define COOLING  55
#define SPARKING 120

static byte heat[NUM_LEDS];

void fire(int frame){
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

byte flashModulus = 8;
void flash(int frame){
  if (frame % flashModulus < 2){
    fill_solid(leds, NUM_LEDS, CRGB(255,255,255));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  }
}

/* Move a dot along the strip */
byte swipeHue = 64;
void swipeSetup(){
  swipeHue = random(255);
}
void swipe(int frame){
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  leds[frame % NUM_LEDS] = CHSV(swipeHue, 255, 128);  
  leds[(frame+1) % NUM_LEDS] = CHSV(swipeHue, 128, 64);  
}

/* Fill and clear the strip*/
byte fillHue = 90;
void fillSetup(){
  fillHue = random(255);
}
void fill(int frame){
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));

  byte b = frame % (NUM_LEDS * 2);

  if (b <= NUM_LEDS) {
    fill_solid(leds, frame % NUM_LEDS, CHSV(fillHue, 255, 128));
  } else {
    b = frame % NUM_LEDS; 
    fill_solid(leds + b, NUM_LEDS - b, CHSV(fillHue, 255, 128));
  }
}

/* Random sparkles */
void sparkles(int frame){
  fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  if(random(2) == 0){
    byte spark = random(NUM_LEDS);
    leds[spark] = CRGB(255,255,255);
  }
}

/* Flash a gradient */
byte gradientModulus = 10;
void gradientSetup () {
  gradientModulus = random(16); 
}
void gradient(int frame){
  if (frame % gradientModulus < 2){
    fill_gradient(leds, 0, CHSV(64,255,128), NUM_LEDS, CHSV(192,255,128));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  }
}

typedef void FUNC(int);

FUNC *patterns[] = {
  gradient,
  sparkles,
  fill,
  flash,
  fire
};

int currentPattern;
int frame = 0;

void loop()
{
  frame++;

  currentPattern = (millis() / 5000) % sizeof(patterns);
  patterns[currentPattern](frame); // run simulation frame
  // fire(frame);
  
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}


