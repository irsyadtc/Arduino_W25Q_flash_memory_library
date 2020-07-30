/***************************************************
*    W25Q Flash Memory Library
*
*    Year: 2020
*    Author: Muhammad Irsyad Sahalan
****************************************************/

#ifndef W25Q_h
#define W25Q_h

#include "Arduino.h"
#include <SPI.h>

// WinBond flash commands
#define WB_WRITE_ENABLE       0x06
#define WB_WRITE_DISABLE      0x04
#define WB_CHIP_ERASE         0xc7
#define WB_READ_STATUS_REG_1  0x05
#define WB_READ_DATA          0x03
#define WB_PAGE_PROGRAM       0x02
#define WB_JEDEC_ID           0x9f




class W25Q
{
  public:
    W25Q();
    void not_busy(void);
    void print_page_bytes(byte *page_buffer);
    void _get_jedec_id(byte *b1, byte *b2, byte *b3);
    void get_jedec_id(void);
    void _chip_erase(void);
    void chip_erase(void);
    void _read_page(word page_number, byte *page_buffer);
    void read_page(unsigned int page_number);
    void read_all_pages(void);
    void _read_page_internal(word page_number, byte *page_buffer);
    void _write_page(word page_number, byte *page_buffer);
    void write_byte(word page, byte offset, byte databyte);
    void printBuffer(void);
    byte getLastByte(void);

  private:
    byte read_buffer[256];

};
#endif
