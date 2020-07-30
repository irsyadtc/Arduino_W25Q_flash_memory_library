/************************************************************
 * 
 * 
 * Mega SPI W25Q write a page
 * 
 * A page consists of 256 bytes (e.g. ****00 to ****FF)
 * 
 * Date: 29/07/20
 * Author: Muhammad Irsyad
 ************************************************************/
 

#include <W25Q.h>

// Mega pinout
// SS:   pin D53
// MOSI: pin D51
// MISO: pin D50
// SCK:  pin D52

int8_t pageno = 1; //page number
int8_t offset = 0;

String data = "Hello World!";


W25Q memory;


void setup(void) {

  Serial.begin(9600);
  Serial.println();
  
  Serial.print("write page: ");
  Serial.println(pageno);
  for(int i = data.length()-1; i >= 0; i--)
  {
     byte buf = data[i];
     Serial.print(buf);
     memory.write_byte(pageno, i, buf);
  }
  Serial.println();
  Serial.println("Finish write");
  
  Serial.print("read page: ");
  Serial.println(pageno);
  memory.read_page(pageno);
  memory.printBuffer();
  Serial.println("Finish");
  Serial.println();


}

void loop(void) {
  
}
