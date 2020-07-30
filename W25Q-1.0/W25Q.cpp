/***************************************************
     W25Q Flash Memory Library

     Year: 2020
     Author: Muhammad Irsyad Sahalan
****************************************************/

#include "Arduino.h"
#include "W25Q.h"


W25Q::W25Q()
{
  SPI.begin();
  SPI.setDataMode(0);
  SPI.setBitOrder(MSBFIRST);
}


void W25Q::not_busy(void)
{
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_STATUS_REG_1);
  while (SPI.transfer(0) & 1) {};
  digitalWrite(SS, HIGH);
}


void W25Q::print_page_bytes(byte *page_buffer) {
  char buf[10];
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 16; ++j) {
      sprintf(buf, "%02x", page_buffer[i * 16 + j]);
    }
  }
}

void W25Q::_get_jedec_id(byte *b1, byte *b2, byte *b3) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_JEDEC_ID);
  *b1 = SPI.transfer(0); // manufacturer id
  *b2 = SPI.transfer(0); // memory type
  *b3 = SPI.transfer(0); // capacity
  digitalWrite(SS, HIGH);
  not_busy();
}

/*
   The JEDEC ID is fairly generic, I use this function to verify the setup
   is working properly.
*/
void W25Q::get_jedec_id(void) {
  Serial.println("command: get_jedec_id");
  byte b1, b2, b3;
  _get_jedec_id(&b1, &b2, &b3);
  char buf[128];
  sprintf(buf, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh",
          b1, b2, b3);
  Serial.println(buf);
  Serial.println("Ready");
}

/*
   See the timing diagram in section 9.2.26 of the
   data sheet, "Chip Erase (C7h / 06h)". (Note:
   either opcode works.)
*/
void W25Q::_chip_erase(void) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_CHIP_ERASE);
  digitalWrite(SS, HIGH);
  /* See notes on rev 2
    digitalWrite(SS, LOW);
    SPI.transfer(WB_WRITE_DISABLE);
    digitalWrite(SS, HIGH);
  */
  not_busy();
}


void W25Q::chip_erase(void) {
  _chip_erase();
}

/*
   See the timing diagram in section 9.2.10 of the
   data sheet, "Read Data (03h)".
*/
void W25Q::_read_page(word page_number, byte *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_DATA);
  // Construct the 24-bit address from the 16-bit page
  // number and 0x00, since we will read 256 bytes (one
  // page).
  SPI.transfer((page_number >> 8) & 0xFF);
  SPI.transfer((page_number >> 0) & 0xFF);
  SPI.transfer(0);

  for (int i = 0; i < 256; ++i) {
    page_buffer[i] = SPI.transfer(0);
    //Serial.print(page_buffer[i]);
  }
  digitalWrite(SS, HIGH);
  not_busy();
  //Serial.println();
}

void W25Q::read_page(unsigned int page_number) {
  char buf[80];
  sprintf(buf, "command: read_page(%04xh)", page_number);
  _read_page(page_number, read_buffer);
  //print_page_bytes(page_buffer);
}

void W25Q::read_all_pages(void) {
  byte page_buffer[256];
  for (int i = 0; i < 4096; ++i) {
    _read_page(i, page_buffer);
    print_page_bytes(page_buffer);
  }
}

void W25Q::_read_page_internal(word page_number, byte *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_DATA);
  // Construct the 24-bit address from the 16-bit page
  // number and 0x00, since we will read 256 bytes (one
  // page).
  SPI.transfer((page_number >> 8) & 0xFF);
  SPI.transfer((page_number >> 0) & 0xFF);
  SPI.transfer(0);

  for (int i = 0; i < 256; ++i) {
    page_buffer[i] = SPI.transfer(0);
  }
  digitalWrite(SS, HIGH);
  not_busy();
}

/*
   See the timing diagram in section 9.2.21 of the
   data sheet, "Page Program (02h)".
*/
void W25Q::_write_page(word page_number, byte *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_PAGE_PROGRAM);
  SPI.transfer((page_number >>  8) & 0xFF);
  SPI.transfer((page_number >>  0) & 0xFF);
  SPI.transfer(0);
  for (int i = 0; i < 256; ++i) {
    SPI.transfer(page_buffer[i]);
  }
  digitalWrite(SS, HIGH);
  
//  for (int i = 0; i < 256; ++i){
//  }
  
  
  /* See notes on rev 2
    digitalWrite(SS, LOW);
    SPI.transfer(WB_WRITE_DISABLE);
    digitalWrite(SS, HIGH);
  */
  not_busy();
}



void W25Q::write_byte(word page, byte offset, byte databyte) {
  char buf[80];
  sprintf(buf, "command: write_byte(%04xh, %04xh, %02xh)", page, offset, databyte);
  byte page_data[256];
  _read_page_internal(page, page_data);
  page_data[offset] = databyte;
  _write_page(page, page_data);
}


/*****************************************************
 *  print out the value on read_buffer
 *****************************************************/
void W25Q::printBuffer(void) {
	for(int i=0; i<255; i++)
	{
		if(read_buffer[i] != 255)
			Serial.print(char(read_buffer[i]));
		else
			break;
	}
	Serial.println();
}

/*****************************************************
 *  get the value of the last byte on read_buffer
 *****************************************************/
byte W25Q::getLastByte(void) {
    byte mark = 0;
    for(int i=0; i<255; i++)
    {
            if(read_buffer[i] == 255)
                {
                    mark = read_buffer[i-1];
                    break;
                }
    }
    return mark;
}

