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
  // TODO: store endpoint address info
  return true;
}

uint8_t CH375USBPrinter::getPortStatus() {
  ch375.toggleHostEndpoint7(false);
  //TODO: refactor and document
  uint8_t buf[8];
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
