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
  free_page_directory = 0;
}


void W25Q::not_busy(void)
{
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_STATUS_REG_1);
  while (SPI.transfer(0) & 1) {};
  digitalWrite(SS, HIGH);
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

/**************************************************
 *  READ
 *************************************************/
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
//    Serial.print(page_buffer[i]);
  }
  digitalWrite(SS, HIGH);
  not_busy();
}

void W25Q::read_page(unsigned int page_number) {
//  char buf[80];
//  sprintf(buf, "command: read_page(%04xh)", page_number);
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


/**************************************************
 *  WRITE
 *************************************************/
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


/**************************************************
 *  PRINT
 *************************************************/

void W25Q::print_page_bytes(byte *page_buffer) {
  char buf[10];
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 16; ++j) {
      sprintf(buf, "%02x", page_buffer[i * 16 + j]);
    }
  }
}

/*****************************************************
 *  print out the value on read_buffer (raw)
 *****************************************************/
void W25Q::printBuffer(void) {
	for(int i=0; i<255; i++)
	{
            Serial.print(char(read_buffer[i]));
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

/***********************************************************
 *  find first empty page and assign to free_page_directory
 **********************************************************/
//need revision
void W25Q::findEmptyPage(uint8_t block_no) {
    unsigned int page_temp = 0;
    page_temp = block_no*256;
    for(int i=0; i<255; i++)
    {
        read_page(page_temp+i);
        if(read_buffer[0] == 0xFF)
            page_temp = page_temp+i;
    }


}

/***********************************************************
 *  print out value on read_buffer in hex format
 **********************************************************/
void W25Q::printBufferHex(void) {
    for(int i=0; i<255; i++)
    {
            if(read_buffer[i] != 255)
                    Serial.print(read_buffer[i],HEX);
            else
                    break;
    }
    Serial.println();
}


/***********************************************************
 *  get the byte of the specified array element of read_buffer
 **********************************************************/
byte W25Q::getRead_buffer(uint8_t b) {
    return read_buffer[b];
}

/***********************************************************
 *  get byte stored in first_byte
 **********************************************************/
byte W25Q::getFirst_byte() const
{
    return first_byte;
}

/***********************************************************
 *  get byte stored in second_byte
 **********************************************************/
byte W25Q::getSecond_byte() const
{
    return second_byte;
}

/***********************************************************
 *  get free page directory
 **********************************************************/
unsigned int W25Q::getFree_page_directory()
{
    return free_page_directory;
}

/***********************************************************
 *  set free page directory
 **********************************************************/
void W25Q::setFree_page_directory(unsigned int value)
{
    free_page_directory = value;
}

/***********************************************************
 *  get content elements
 **********************************************************/
 uint8_t W25Q::getContent_size()
{
    return content_size;
}

/***********************************************************
 *  get content elements
 **********************************************************/
byte W25Q::getContent(uint8_t index)
{
    return content[index];
}


/***********************************************************
 *  clear content
 **********************************************************/
void W25Q::clearContent()
{
    for(uint8_t i=0; i<254; i++)
    {
        content[i] = 0;
    }
}

void W25Q::extractContent()
{
      first_byte = read_buffer[0];      //extract first byte
      second_byte = read_buffer[1];     //extract second byte
      clearContent();
      for(uint8_t i=2; i<256 ;i++)    //extract content
      {
          if(read_buffer[i] == 0xFF){
              content_size = i-2;   //calc content size
              break;
          }
          else{
              content[i-2] = read_buffer[i];
              if(i ==255)
                  content_size = i-2;
          }

      }
}
