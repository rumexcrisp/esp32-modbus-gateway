// =================================================================================================
// eModbus: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to ModbusClient
//               MIT license - see license.md for details
// =================================================================================================
// Includes: <Arduino.h> for Serial etc., WiFi.h for WiFi support
#include <Arduino.h>
#include "HardwareSerial.h"
#include <WiFi.h>

// Modbus bridge include
#include "ModbusBridgeWiFi.h"
// Modbus RTU client include
#include "ModbusClientRTU.h"

#ifndef MY_SSID
#define MY_SSID "||=Fritz!Box=||2"
#endif
#ifndef MY_PASS
#define MY_PASS "asdfghjkl67@76!"
#endif

char ssid[] = MY_SSID;                     // SSID and ...
char pass[] = MY_PASS;                     // password for the WiFi network used
uint16_t port = 502;                       // port of modbus server

// Create a ModbusRTU client instance
ModbusClientRTU RS485(Serial2);

void handleData(ModbusMessage msg, uint32_t token) 
{
  Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", msg.getServerID(), msg.getFunctionCode(), token, msg.size());
  for (auto& byte : msg) {
    Serial.printf("%02X ", byte);
  }
  Serial.println("");
  Serial.printf("Response: %s\n", (const char *)msg.data());
}

void handleError(Error error, uint32_t token) 
{
  // ModbusError wraps the error code and provides a readable error message for it
  ModbusError me(error);
  Serial.printf("Error response: %02X - %s\n", error, (const char *)me);
}

// Setup() - initialization happens here
void setup() {
// Init Serial monitor
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("__ OK __");

  // Set up Serial2 connected to Modbus RTU
  Serial2.begin(9600, SERIAL_8N1);

  // Set up ModbusClientRTU client.
  // - provide onData and onError handler functions
  RS485.onDataHandler(&handleData);
  RS485.onErrorHandler(&handleError);

  // Set message timeout to 2000ms
  RS485.setTimeout(2000);

  // Start ModbusClientRTU background task
  RS485.begin(1);

  Serial.write("Done with setup");
}

// loop() - nothing done here today!
void loop() {
  if(Serial.available()) {
    // Serial2.write(Serial.read());
    int value1 = Serial.parseInt();
    Serial.printf("Input: %d\n", value1);
    int value2 = Serial.parseInt();
    Serial.printf("Input: %d\n", value2);
    Error err = RS485.addRequest(0x12345678, 1, READ_HOLD_REGISTER, value1, value2); // 16384
    if (err!=SUCCESS) {
      ModbusError e(err);
      Serial.printf("Error creating request: %02X - %s\n", err, (const char *)e);
    }
  }
  // delay(1000);
}