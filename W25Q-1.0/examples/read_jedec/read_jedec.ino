/************************************************************
 * CAREM UTP 2020
 * 
 * Mega SPI W25Q display JEDEC ID
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
  Serial.println("Setting up");
  
  memory.get_jedec_id();
  
  Serial.println("READY");
  Serial.println();


}

void loop(void) {
  
}
