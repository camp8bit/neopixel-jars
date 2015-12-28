// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(500);

void setup()
{
    Serial.begin(9600);    // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

byte b;

void loop()                                                                                                                                                                                              
{
    const char *msg = "hello";
    b = millis() / 5000;
    
    // driver.send((uint8_t *)msg, strlen(msg));
    driver.send((uint8_t *)&b, 1);
    driver.waitPacketSent();
    delay(100);
    Serial.println(b, HEX);
}
