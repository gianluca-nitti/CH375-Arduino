#include "CH375USBPrinter.h"

CH375USBPrinter::CH375USBPrinter(CH375& _ch375): ch375(_ch375) {
}

bool CH375USBPrinter::init() {
  USBDeviceDescriptor deviceDescriptor;
  if (!ch375.resetAndGetDeviceDescriptor(&deviceDescriptor)) return false;
  if (deviceDescriptor.bDeviceClass != 0 || deviceDescriptor.bDeviceSubClass != 0 || deviceDescriptor.bDeviceProtocol != 0) return false;
  if (!ch375.setAddress(3)) return false;
  USBConfigurationDescriptorFull configurationDescriptor;
  if (!ch375.getFullConfigurationDescriptor(&configurationDescriptor)) return false;
  if (configurationDescriptor.interface.bInterfaceClass != USB_PRINTER_INTERFACE_CLASS
    || configurationDescriptor.interface.bInterfaceSubClass != USB_PRINTER_INTERFACE_SUBCLASS) return false;
  if (!ch375.setConfiguration(configurationDescriptor.configuration.bConfigurationValue)) return false;
  // find the OUT endpoint
  for (uint8_t i = 0; i < configurationDescriptor.interface.bNumEndpoints; i++) {
    // according to USB specification, the OUT endpoint's address most significant bit is 0;
    // if there is an IN endpoint, it's address has the most significant bit set to 1
    if ((configurationDescriptor.endpoints[i].bEndpointAddress & 0b10000000) == 0) {
      // less significant 4 bits of the address are the endpoint number
      outEndpointNumber = configurationDescriptor.endpoints[i].bEndpointAddress & 0x0F;
      // the wMaxPacketSize field is 2 bytes large, but the CH375 send buffer size is 64 bytes;
      // thus, if the printer supports larger packets, the bottleneck is the CH375
      uint16_t maxPacketSize = configurationDescriptor.endpoints[i].wMaxPacketSize;
      if (maxPacketSize > 64) {
        outEndpointMaxPacketSize = 64;
      } else {
        outEndpointMaxPacketSize = (uint8_t) (maxPacketSize & 0x00FF);
      }
      return true;
    }
  }
  return false; //no OUT endpoint found
}

uint8_t CH375USBPrinter::getPortStatus() {
  ch375.toggleHostEndpoint7(false);
  //TODO: refactor and document
  uint8_t buf[8] = {0};
  buf[0] = 0b10100001; //bmRequestType
  buf[1] = 1; //bRequest
  buf[6] = 1; //wLength
  ch375.wr_usb_data(buf, 8);
  if (!ch375.issueToken(0, USB_PID_SETUP)) return 0xFF; //SETUP phase
  ch375.toggleHostEndpoint6(true);
  if (!ch375.issueToken(0, USB_PID_IN)) return 0xFF; //DATA phase
  uint8_t result;
  ch375.rd_usb_data(&result, 1);
  ch375.toggleHostEndpoint7(true);
  ch375.wr_usb_data(NULL, 0); //send 0-length data to report that control transfer was successful
  if (!ch375.issueToken(0, USB_PID_OUT)) return 0xFF;
  return result;
}

bool CH375USBPrinter::sendData(uint8_t* buf, uint8_t len) {
  while (len > 0) {
    ch375.toggleHostEndpoint7(toggleSend);
    uint8_t packetLen = len > outEndpointMaxPacketSize ? outEndpointMaxPacketSize : len;
    ch375.wr_usb_data(buf, packetLen);
    if (ch375.issueToken(outEndpointNumber, USB_PID_OUT)) {
      len -= packetLen;
      buf += packetLen;
      toggleSend = !toggleSend;
    } else {
      return false;
    }
    //TODO: handle STALL and NAK situations
  }
  return true;
}
