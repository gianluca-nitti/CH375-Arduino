#pragma once
#include "Arduino.h"

#define CH375_CMD_GET_IC_VER 0x01
#define CH375_CMD_CHECK_EXIST 0x06
#define CH375_CMD_SET_USB_ADDR 0x13
#define CH375_CMD_SET_USB_MODE 0x15
#define CH375_CMD_GET_STATUS 0x22
#define CH375_CMD_RD_USB_DATA 0x28
#define CH375_CMD_SET_ADDRESS 0x45
#define CH375_CMD_GET_DESCR 0x46

#define CH375_CMD_RET_SUCCESS 0x51

#define CH375_USB_MODE_HOST 0x06
#define CH375_USB_MODE_HOST_RESET 0x07

#define CH375_USB_INT_SUCCESS 0x14
#define CH375_USB_INT_CONNECT 0x15

#define CH375_USB_DEVICE_DESCRIPTOR 0x01
#define CH375_USB_CONFIGURATION_DESCRIPTOR 0x02
#define CH375_USB_INTERFACE_DESCRIPTOR 0x04
#define CH375_USB_ENDPOINT_DESCRIPTOR 0x05

typedef struct __attribute__((__packed__)) {
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

typedef struct __attribute__((__packed__)) {
  byte bLength;
  byte bDescriptorType;
  unsigned short wTotalLength;
  byte bNumInterfaces;
  byte bConfigurationValue;
  byte iConfiguration;
  byte bmAttributes;
  byte bMaxPower;
} USBConfigurationDescriptor;

typedef struct __attribute__((__packed__)) {
  byte bLength;
  byte bDescriptorType;
  byte bInterfaceNumber;
  byte bAlternateSetting;
  byte bNumEndpoints;
  byte bInterfaceClass;
  byte bInterfaceSubClass;
  byte bInterfaceProtocol;
  byte iInterface;
} USBInterfaceDescriptor;

typedef struct __attribute__((__packed__)) {
  byte bLength;
  byte bDescriptorType;
  byte bEndpointAddress;
  byte bmAttributes;
  unsigned short wMaxPacketSize;
  byte bInterval;
} USBEndpointDescriptor;

typedef struct __attribute__((__packed__)) {
  USBConfigurationDescriptor configuration;
  USBInterfaceDescriptor interface;
  USBEndpointDescriptor endpoints[4];
} USBConfigurationDescriptorFull;

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
    bool setAddress(byte address);
    bool getFullConfigurationDescriptor(USBConfigurationDescriptorFull* result);
};
