#pragma once
#include "Arduino.h"
#include <functional>

#define CH375_CMD_GET_IC_VER 0x01
#define CH375_CMD_SET_BAUDRATE 0x02
#define CH375_CMD_CHECK_EXIST 0x06
#define CH375_CMD_SET_USB_ADDR 0x13
#define CH375_CMD_SET_USB_MODE 0x15
#define CH375_CMD_SET_ENDP6 0x1C
#define CH375_CMD_SET_ENDP7 0x1D
#define CH375_CMD_GET_STATUS 0x22
#define CH375_CMD_RD_USB_DATA 0x28
#define CH375_CMD_WR_USB_DATA7 0x2B
#define CH375_CMD_SET_ADDRESS 0x45
#define CH375_CMD_GET_DESCR 0x46
#define CH375_CMD_SET_CONFIG 0x49
#define CH375_CMD_ISSUE_TOKEN 0x4F

#define CH375_CMD_RET_SUCCESS 0x51

#define CH375_USB_MODE_HOST 0x06
#define CH375_USB_MODE_HOST_RESET 0x07

#define CH375_USB_INT_SUCCESS 0x14
#define CH375_USB_INT_CONNECT 0x15

#define CH375_USB_DEVICE_DESCRIPTOR 0x01
#define CH375_USB_CONFIGURATION_DESCRIPTOR 0x02
#define CH375_USB_INTERFACE_DESCRIPTOR 0x04
#define CH375_USB_ENDPOINT_DESCRIPTOR 0x05

#define USB_PID_OUT 0x01
#define USB_PID_IN 0x09
#define USB_PID_SETUP 0x0D

typedef struct __attribute__((__packed__)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} USBDeviceDescriptor;

typedef struct __attribute__((__packed__)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} USBConfigurationDescriptor;

typedef struct __attribute__((__packed__)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} USBInterfaceDescriptor;

typedef struct __attribute__((__packed__)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
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
    bool toggleSend = false; //toggle DATA0/DATA1 for sending

    void sendCommand(uint8_t b);
    void sendData(uint8_t b);
    uint8_t receive();
    bool execCommand(uint8_t cmd, uint8_t arg);
    uint8_t waitInterrupt();
    bool getDescriptor(uint8_t descriptorType);
    void toggleHostEndpoint(uint8_t setEndpointCommand, bool tog);
  public:
    CH375(Stream& _stream, int _interruptPin);
    bool init();
    bool test();
    bool setBaudRate(uint32_t baudRate, std::function<void(void)> setLocalBaudRate);
    uint8_t getChipVersion();
    void rd_usb_data(uint8_t* buf, uint8_t maxLen);
    void wr_usb_data(uint8_t* buf, uint8_t len);
    bool resetAndGetDeviceDescriptor(USBDeviceDescriptor* result);
    bool setAddress(uint8_t address);
    bool getFullConfigurationDescriptor(USBConfigurationDescriptorFull* result);
    bool setConfiguration(uint8_t configuration);
    bool issueToken(uint8_t targetEndpoint, uint8_t pid);
    void toggleHostEndpoint6(bool tog);
    void toggleHostEndpoint7(bool tog);
    bool doBulkOutTransfer(uint8_t targetEndpoint, uint8_t* buf, uint8_t len);
};
