#pragma once
#include "Arduino.h"

#define CH375_CMD_GET_IC_VER 0x01
#define CH375_CMD_CHECK_EXIST 0x06
#define CH375_CMD_SET_USB_MODE 0x15
#define CH375_CMD_GET_STATUS 0x22
#define CH375_CMD_RD_USB_DATA 0x28
#define CH375_CMD_GET_DESCR 0x46

#define CH375_CMD_RET_SUCCESS 0x51

#define CH375_USB_MODE_HOST 0x06
#define CH375_USB_MODE_HOST_RESET 0x07

#define CH375_USB_INT_SUCCESS 0x14
#define CH375_USB_INT_CONNECT 0x15

#define CH375_USB_DEVICE_DESCRIPTOR 0x01

typedef struct {
    byte bLength;
    byte bDescriptorType;
    unsigned short bcdUSB;
    byte bDeviceClass;
    byte bDeviceSubClass;
    byte bDeviceProtocol;
    byte bMaxPacketSize;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    byte iManufacturer;
    byte iProduct;
    byte iSerialNumber;
    byte bNumConfigurations;
} USBDeviceDescriptor;

class CH375 {
  private:
    Stream& stream;
    int interruptPin;
    void sendCommand(byte b);
    void sendData(byte b);
    byte receive();
    bool execCommand(byte cmd, byte arg);
    byte waitInterrupt();
    void rd_usb_data(byte* buf, size_t maxLen);
    bool getDescriptor(byte descriptorType);
  public:
    CH375(Stream& _stream, int _interruptPin);
    bool init();
    byte getChipVersion();
    bool resetAndGetDeviceDescriptor(USBDeviceDescriptor* result);
};
