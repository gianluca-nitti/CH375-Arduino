#include "CH375.h"

CH375::CH375(Stream& _stream, int _interruptPin): stream(_stream), interruptPin(_interruptPin) {
}

void CH375::sendCommand(byte b) {
  delay(2);
  Serial.print("Sending: 0x");
  Serial.println(b, HEX);
  stream.write(b);
  delay(2);
}

void CH375::sendData(byte b) {
  stream.write(b);
  Serial.print("Sending: 0x");
  Serial.println(b, HEX);
  delay(2);
}

byte CH375::receive() {
  delayMicroseconds(1);
  while (stream.available() <= 0) yield();
  byte b = stream.read();
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

byte CH375::getChipVersion() {
  sendCommand(CH375_CMD_GET_IC_VER);
  return receive();
}

bool CH375::execCommand(byte cmd, byte arg) {
  sendCommand(cmd);
  sendData(arg);
  delayMicroseconds(20);
  return receive() == CH375_CMD_RET_SUCCESS;
}

byte CH375::waitInterrupt() {
  while (digitalRead(interruptPin) != LOW) yield();
  sendCommand(CH375_CMD_GET_STATUS);
  return receive();
}

void CH375::rd_usb_data(byte* buf, size_t maxLen) {
  sendCommand(CH375_CMD_RD_USB_DATA);
  byte len = receive();
  for (byte i = 0; i < len; i++) {
    byte data = receive();
    if (i < maxLen) {
      buf[i] = data;
    }
  }
}

bool CH375::getDescriptor(byte descriptorType) {
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
  rd_usb_data((byte*)result, sizeof(USBDeviceDescriptor));
  return true;
}
