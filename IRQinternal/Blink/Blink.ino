/////////////////////////////////////////////////////////////////////////////////////////////
//
// - copy-paste header (.h) file of tune you want to play it in sketch folder (look in folders of internal version)
// - all checks are off, check if tune is playable in internal version first
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include"Supremacy.h"

#define GENEROUS  0 // set amount of uS per IRQ that generously will be given to your sketch (greater the number, your sketch will run faster, but sound quality will degrade )
#define FREE_RAM  2048  // set amount of ram that will be available to your sketch (the more ram you have, smaller sid file can be played) // default 2048
volatile uint8_t Tune = 0; // in this global variable will be tune number currently playing when you sketch start. Use it to change tune number playing.

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Your sketch now
//
//////////////////////////////////////////////////////////////////////////////////////////////





void MySetup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT);

}


void MyLoop() {
  // put your main code here, to run repeatedly:

  digitalWrite(LED_BUILTIN, LOW);

  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);

  delay(1000);


}
