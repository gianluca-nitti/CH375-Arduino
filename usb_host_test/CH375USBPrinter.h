#pragma once
#include "CH375.h"

#define USB_PRINTER_INTERFACE_CLASS 0x07
#define USB_PRINTER_INTERFACE_SUBCLASS 0x01

class CH375USBPrinter {
  private:
    CH375& ch375;
  public:
    CH375USBPrinter(CH375& _ch375);
    bool init();
    uint8_t getPortStatus();
};
