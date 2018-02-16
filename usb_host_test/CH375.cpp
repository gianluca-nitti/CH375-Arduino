#include "CH375.h"

CH375::CH375(Stream& _stream, int _interruptPin): stream(_stream), interruptPin(_interruptPin) {
}

void CH375::sendCommand(uint8_t b) {
  delay(2);
  Serial.print("Sending: 0x");
  Serial.println(b, HEX);
  stream.write(b);
  delay(2);
}

void CH375::sendData(uint8_t b) {
  stream.write(b);
  Serial.print("Sending: 0x");
  Serial.println(b, HEX);
  delay(2);
}

uint8_t CH375::receive() {
  delayMicroseconds(1);
  while (stream.available() <= 0) yield();
  uint8_t b = stream.read();
  Serial.print("Received: 0x");
  Serial.println(b, HEX);
  return b;
}

bool CH375::init() {
  pinMode(interruptPin, INPUT);
  delay(200);
  sendCommand(CH375_CMD_CHECK_EXIST);
  sendData(0xCC);
  return receive() == 0x33;
}

uint8_t CH375::getChipVersion() {
  sendCommand(CH375_CMD_GET_IC_VER);
  return receive();
}

bool CH375::execCommand(uint8_t cmd, uint8_t arg) {
  sendCommand(cmd);
  sendData(arg);
  delayMicroseconds(20);
  return receive() == CH375_CMD_RET_SUCCESS;
}

uint8_t CH375::waitInterrupt() {
  while (digitalRead(interruptPin) != LOW) yield();
  sendCommand(CH375_CMD_GET_STATUS);
  return receive();
}

void CH375::rd_usb_data(uint8_t* buf, size_t maxLen) {
  sendCommand(CH375_CMD_RD_USB_DATA);
  uint8_t len = receive();
  for (uint8_t i = 0; i < len; i++) {
    uint8_t data = receive();
    if (i < maxLen) {
      buf[i] = data;
    }
  }
}

bool CH375::getDescriptor(uint8_t descriptorType) {
  sendCommand(CH375_CMD_GET_DESCR);
  sendData(descriptorType);
  return waitInterrupt() == CH375_USB_INT_SUCCESS;
}

bool CH375::resetAndGetDeviceDescriptor(USBDeviceDescriptor* result) {
  if (!execCommand(CH375_CMD_SET_USB_MODE, CH375_USB_MODE_HOST)) return false;
  while (waitInterrupt() != CH375_USB_INT_CONNECT) yield(); //wait for device to connect
  if (!execCommand(CH375_CMD_SET_USB_MODE, CH375_USB_MODE_HOST_RESET)) return false; //perform reset
  delay(10);
  if (!execCommand(CH375_CMD_SET_USB_MODE, CH375_USB_MODE_HOST)) return false;
  delay(100);
  while (waitInterrupt() != CH375_USB_INT_CONNECT) yield(); //wait for device to come back online after reset
  delay(200);
  if (!getDescriptor(CH375_USB_DEVICE_DESCRIPTOR)) return false;
  rd_usb_data((uint8_t*)result, sizeof(USBDeviceDescriptor));
  return result->bLength >= 18 //device descriptor length should be 18
          && result->bDescriptorType == CH375_USB_DEVICE_DESCRIPTOR //ensure it's actually a device descriptor
          && result->bNumConfigurations == 1; //devices with more than 1 configuration are not supported (for now at least)
}

bool CH375::setAddress(uint8_t address) {
  if (address & 0b10000000) return false; //first bit must be zero (USB addresses are 7 bit long)
  sendCommand(CH375_CMD_SET_ADDRESS); //assign this address to the device
  sendData(address);
  if (waitInterrupt() != CH375_USB_INT_SUCCESS) return false;
  sendCommand(CH375_CMD_SET_USB_ADDR); //instruct the CH375 (USB host) to talk to the device with this address
  sendData(address);
  delay(5);
  return true;
}

bool CH375::getFullConfigurationDescriptor(USBConfigurationDescriptorFull* result) {
  if(!getDescriptor(CH375_USB_CONFIGURATION_DESCRIPTOR)) return false;
  rd_usb_data((uint8_t*)result, sizeof(USBConfigurationDescriptorFull));
  return result->configuration.bDescriptorType == CH375_USB_CONFIGURATION_DESCRIPTOR //ensure configuration descriptor is actually a configuration descriptor
          && result->configuration.bNumInterfaces == 1 //only one interface is supported (for now at least)
          && result->interface.bDescriptorType == CH375_USB_INTERFACE_DESCRIPTOR //ensure interface descriptor is actually an interface descriptor
          && result->interface.bNumEndpoints <= 4; //a maximum of 4 endpoints are supported
}

bool CH375::setConfiguration(uint8_t configuration) {
  //TODO: set toggle send/toggle receive flag?
  sendCommand(CH375_CMD_SET_CONFIG);
  sendData(configuration);
  return waitInterrupt() == CH375_USB_INT_SUCCESS;
}
