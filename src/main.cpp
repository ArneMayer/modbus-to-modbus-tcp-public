#include <ESP8266WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>
#include <SoftwareSerial.h>

#define SLAVE_ID 1
#define PULL_ID 1

ModbusRTU mb1;
ModbusIP mb2;

// D5 is connected to RXD on the RS485 board 
// and D6 ist connected to TXD
SoftwareSerial softSerial(D5, D6); // (RX, TX)

void setup() {
  Serial.begin(74880);
  softSerial.begin(9600, SWSERIAL_8N1);

  WiFi.hostname("esp-modbus");
  WiFi.begin("todo-ssid", "todo-pw");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  mb1.begin(&softSerial, D4);  // Specify RE_DE control pin
  mb1.master();
  auto port = 502;
  mb2.server(port);

  mb2.addHreg(0, 0, 256);

  Serial.print("Listening on port ");
  Serial.println(port);
}

void waitForResult() {
    while(mb1.slave()) {
      mb1.task();
      mb2.task();
      delay(1);
    }
}

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  return true;
}

void loop() {
  while(true) {
    waitForResult();
    mb1.pullHreg(PULL_ID, 0x00, 0x00, 0x01-0x00 + 1, cbWrite);
    waitForResult();
    mb1.pullHreg(PULL_ID, 0x08, 0x08, 0x15-0x08 + 1, cbWrite);
    waitForResult();
    mb1.pullHreg(PULL_ID, 0x80, 0x80, 0x8F-0x80 + 1, cbWrite);
    waitForResult();
    mb1.pullHreg(PULL_ID, 0x90, 0x90, 0x98-0x90 + 1, cbWrite);
  }
}