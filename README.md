# CH375-Arduino

* This is an open source library for the Arduino development environment which simplifies the interaction with the CH375 USB controller chips.
* Currently in very early development stage:
	* The CH375 supports communication with a microcontroller both via UART serial and via an 8-bit parallel bust, but at the moment onle the serial mode is supported.
	* Only supports USB host mode (CH375 can also work as a USB device but this is not supported yet and not a priority).
* The goal is to provide a low-level API for USB requests (control transfers, bulk transfers, etc) as well as high-level APIs for the most common [USB device classes](https://en.wikipedia.org/wiki/USB#Device_classes) (printer, HID, mass storage, etc.).
	* The low-level API is provided by the CH375 class (see CH375.h and CH375.cpp)
	* At the moment the only available high-level API is for the [Printer device class](https://cscott.net/usb_dev/data/devclass/usbprn10.pdf) (see CH375USBPrinter.h and CH375USBPrinter.cpp)
* Currently the main target microcontroller is the ESP8266; tests are done using the [EspSoftwareSerial](https://github.com/plerup/espsoftwareserial) library by Peter Lerup for ESP8266-CH375 communication. The example compiles for the AVR architecture as well but no testing has been done on AVR boards yet.

## Useful links for development

### General information about USB
* [Official docs](http://www.usb.org/developers/docs/usb20_docs/)
* [usbmadesimple.co.uk](http://www.usbmadesimple.co.uk/)
* [beyondlogic.org](http://www.beyondlogic.org/usbnutshell/usb1.shtml)

### CH375 documentation
Unfortunately there isn't a lot of English documentation about this chip. A lot of material that can be found on the manufacturer's website is only available in Chinese (which I don't understand), however I found that Google Translate can produce a quite readable English from it.
* [English datasheet](http://www.electrodragon.com/w/images/1/18/CH375DS1.PDF)
* [Official webpage](http://www.wch.cn/product/CH375.html) - I used Google Translate to get to the Downloads section at the bottom of the page. There are some zips with C code examples for other microcontrollers (Intel MCS-51). Said source files have function and variable names in English but comments in Chinese. Copying and pasting these comments in Google Translate can give a decent understanding of what the code does.
