/************************************************************
 * CAREM UTP 2020
 * 
 * Mega SPI W25Q clear all memory
 * 
 * Author: Muhammad Irsyad
 ************************************************************/
 

#include <W25Q.h>

// Mega pinout
// SS:   pin D53
// MOSI: pin D51
// MISO: pin D50
// SCK:  pin D52

W25Q memory;


void setup(void) {

  Serial.begin(9600);
  Serial.println("");
  Serial.println("WARNING: Clear all memory!");
  delay(5000);       //a delay time to turn off system in case you mistakenly upload this
  Serial.println("WARNING: START CLEAR ALL!");
  memory._chip_erase();
  Serial.println("Finish");
  Serial.println();


}

void loop(void) {
  
}
