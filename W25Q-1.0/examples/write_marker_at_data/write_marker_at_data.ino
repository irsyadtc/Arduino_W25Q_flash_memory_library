/************************************************************

   Mega SPI W25Q read the last written byte for data marker

   If the last written byte is 0xFE, then the data is invalid.
   Else it is consider useful
   
   A page consists of 256 bytes (e.g. ****00 to ****FF)

   Date: 30/07/20
   Author: Muhammad Irsyad Sahalan
 ************************************************************/


#include <W25Q.h>

// Mega pinout
// SS:   pin D53
// MOSI: pin D51
// MISO: pin D50
// SCK:  pin D52

int8_t pageno = 10; //page number
int8_t offset = 0;

String data = "Hello World!";


W25Q memory;


void setup(void) {

  Serial.begin(9600);
  Serial.println();

  //write
  Serial.print("write data to page: ");
  Serial.println(pageno);
  for (int i = data.length() - 1; i >= 0; i--)
  {
    byte buf = data[i];
    Serial.print(buf);
    memory.write_byte(pageno, i, buf);
  }
  Serial.println();
  Serial.println("Finish write");

  //read
  Serial.print("1st read page: ");
  Serial.println(pageno);
  memory.read_page(pageno);
  memory.printBuffer();

  //comparison
  if(memory.getLastByte() == 254)
    Serial.println("invalid");
  else
    Serial.println("valid");

  //mark page
  Serial.print("1st write mark to page: ");
  Serial.println(pageno);
  memory.write_byte(pageno, data.length(), 0xFE);

  //read back
  Serial.print("2nd read page: ");
  Serial.println(pageno);
  memory.read_page(pageno);
  memory.printBuffer();

  //comparison
  if(memory.getLastByte() == 254)
    Serial.println("invalid");
  else
    Serial.println("valid");

  Serial.println("Finish");
  Serial.println();


}

void loop(void) {

}
