#include <SoftwareSerial.h>
#include "CH375.h"

SoftwareSerial swSer(D6, D3, false, 32);
CH375 ch375(swSer, D4);

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
  ch375.setBaudRate(115200, [](){
    swSer.begin(115200);
  });
}

void loop() {
	Serial.println("Testing CH375...");
	bool test = ch375.test();
	Serial.println(test ? "OK" : "ERROR");
	if (!test) {
		while (true) delay(1000);
	}
	//delay(1000);
}
