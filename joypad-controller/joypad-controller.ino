#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;

#define JOYPIN 1

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "Camp 8bit";
int wait = 50; // In milliseconds

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

void setup() {
  matrix.setIntensity(15); // Use a value between 0 and 15 for brightness

// Adjust to your own needs
//  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
//  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
//  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
//  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
//  ...
//  matrix.setRotation(0, 2);    // The first display is position upside down
//  matrix.setRotation(3, 2);    // The same hold for the last display
}

int mode = 0;
int vader = 0;

void loop() {

  // value1 = analogRead(joyPin1);  
  int i = 1024 - analogRead(JOYPIN);
//
//  Serial.print(i, DEC);
//  Serial.println("");

  if (i < 256) {
    mode--;
    delay(40);
    vader = -10;
  }

  if (i > 768) {
    mode++;
    delay(40);
    vader = -10;
  }

  mode = max(mode, 0);
  mode = min(mode, 25);
  
  // i = i * (width * tape.length() + matrix.width() - 1 - spacer) / 1024;
  
//for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

  int x = 2;
  int y = 1;
  
  // matrix.setIntensity(vader % 16); // (byte) sin((float) vader / 16.0) * 6 + 9);

  if (false) { // (vader % 50 == 0){
    matrix.fillScreen(HIGH);
    //matrix.drawChar(x, y, 2, LOW, HIGH, 1); // smiley face
    matrix.drawChar(x, y, mode + 'A', LOW, HIGH, 1);
  } else {
    matrix.fillScreen(LOW);
    matrix.drawChar(x, y, mode + 'A', HIGH, LOW, 1);
    // matrix.drawChar(x, y, mode, HIGH, LOW, 1);
  }

  vader++;

  // matrix.drawPixel(vader % 8, 7, HIGH);
  
  matrix.write(); // Send bitmap to display

  delay(40);
}

