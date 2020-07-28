/************************************************************
 * CAREM UTP 2020
 * 
 * Mega SPI W25Q read a page
 * 
 * A page consists of 256 bytes (e.g. ****00 to ****FF)
 * 
 * Author: Muhammad Irsyad
 ************************************************************/
 

#include <W25Q.h>

// Mega pinout
// SS:   pin D53
// MOSI: pin D51
// MISO: pin D50
// SCK:  pin D52

int8_t pageno = 1; //page number

W25Q memory;


void setup(void) {

  Serial.begin(9600);
  Serial.println("");
  Serial.print("read page: ");
  Serial.println(pageno);
  memory.read_page(pageno);
  memory.printBuffer();
  Serial.println("Finish");
  Serial.println();


}

void loop(void) {
  
}
