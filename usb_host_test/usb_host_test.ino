#include <SoftwareSerial.h>
#include "CH375.h";
#include "CH375USBPrinter.h"

SoftwareSerial swSer(D6, D3, false, 32);
CH375 ch375(swSer, D4);
CH375USBPrinter printer(ch375);

void setup() {
  Serial.begin(115200);
  Serial.println("preparing...");
  delay(5000);
  Serial.println("ready");
  swSer.begin(9600);
  if (ch375.init()) {
    Serial.println("CH375 test OK");
  } else {
    Serial.println("CH375 is not working properly");
    while (true) delay(1000);
  }
}

void printHex(String msg, int data) {
  Serial.print(msg + ": 0x");
  Serial.println(data, HEX);
}

void waitSecs(int secs) {
  for (int i = secs; i > 0; i--) {
    Serial.printf("%d...", i);
    delay(1000);
  }
}

void loop() {
  delay(1000);
  Serial.print("Chip version: 0x");
  Serial.println(ch375.getChipVersion(), HEX);

  Serial.println("Waiting for device...");
  USBDeviceDescriptor deviceDescriptor;
  if (ch375.resetAndGetDeviceDescriptor(&deviceDescriptor)) {
    Serial.println("USB device descriptor: ");
    printHex("bLength", deviceDescriptor.bLength);
    printHex("bDescriptorType", deviceDescriptor.bDescriptorType);
    printHex("bcdUSB", deviceDescriptor.bcdUSB);
    printHex("bDeviceClass", deviceDescriptor.bDeviceClass);
    printHex("bDeviceSubClass", deviceDescriptor.bDeviceSubClass);
    printHex("bDeviceProtocol", deviceDescriptor.bDeviceProtocol);
    printHex("bMaxPacketSize", deviceDescriptor.bMaxPacketSize);
    printHex("idVendor", deviceDescriptor.idVendor);
    printHex("idProduct", deviceDescriptor.idProduct);
    printHex("bdcDevice", deviceDescriptor.bcdDevice);
    printHex("iManufacturer", deviceDescriptor.iManufacturer);
    printHex("iProduct", deviceDescriptor.iProduct);
    printHex("iSerialNumber", deviceDescriptor.iSerialNumber);
    printHex("bNumConfigurations", deviceDescriptor.bNumConfigurations);

    Serial.println("Initializing as printer...");
    if (printer.init()) {
      Serial.println("Printer intialization OK");
      Serial.println("Port status: ");
      Serial.println(printer.getPortStatus(), BIN);
      Serial.println("Starting to print in ");
      waitSecs(5);
      // simple document in HP page description language
      uint8_t dataToPrint[] = {0x1B, 'E', '\n', '\n', '\n', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\n', 0x1B, 'E'};
      uint8_t len = sizeof(dataToPrint) / sizeof(dataToPrint[0]);
      if(printer.sendData(dataToPrint, len)) {
        Serial.println("Succesfully printed!");
        while (true) delay(1000);
      } else {
        Serial.println("Failed to send data to the printer");
      }
    } else {
      Serial.println("Failed");
    }

    /*Serial.println("Setting device address to 3...");
      if (ch375.setAddress(3)) {
      Serial.println("Address set");
      Serial.println("Reading configuration descriptor...");
      USBConfigurationDescriptorFull configurationDescriptor;
      if (ch375.getFullConfigurationDescriptor(&configurationDescriptor)) {
        Serial.println("USB configuration descriptor read OK.");
        printHex("bInterfaceClass", configurationDescriptor.interface.bInterfaceClass);
        printHex("bInterfaceSubClass", configurationDescriptor.interface.bInterfaceSubClass);
        Serial.println("Setting configuration...");
        if (ch375.setConfiguration(configurationDescriptor.configuration.bConfigurationValue)) {
          Serial.println("Configuration set");
        } else {
          Serial.println("Failed to set configuration");
        }
      } else {
        Serial.println("Failed to read configuration descriptor");
      }
      } else {
      Serial.println("Failed to set address");
      }*/
  } else {
    Serial.println("Failed");
  }
  waitSecs(10);
}
