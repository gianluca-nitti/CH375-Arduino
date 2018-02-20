#pragma once
#include "CH375.h"

#define USB_PRINTER_INTERFACE_CLASS 0x07
#define USB_PRINTER_INTERFACE_SUBCLASS 0x01

class CH375USBPrinter {
  private:
    CH375& ch375;
    uint8_t outEndpointNumber;
    uint8_t outEndpointMaxPacketSize;
    bool toggleSend = false;
  public:
    CH375USBPrinter(CH375& _ch375);
    bool init();
    uint8_t getPortStatus();
    bool sendData(uint8_t* buf, uint8_t len);
};
