#include <SoftwareSerial.h>
#include <DHT.h>
#include <LowPower.h>

#include "Conf.hpp"

namespace glob
{
  DHT Dht = DHT(Conf::dhtPin, Conf::dhtType);
  HardwareSerial& Serial = ::Serial;
}

void deepSleepFor8s(int times = 1) {
  int oldClkPr = CLKPR;
  CLKPR = 0x80;
  CLKPR = 0x08;
  for (int i = 0; i < times; ++i) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  CLKPR = 0x80;
  CLKPR = oldClkPr;
}

bool sendCommand(const String& command, int maxTries, char* expectedResult) {
  int triesCount = 0;

  while(triesCount < maxTries) {
    glob::Serial.println(command);
    if(glob::Serial.find(expectedResult)) {
      return true;
    }

    ++triesCount;
  }

  setErrorMode(true);
  return false;
}

void setErrorMode(bool val) {
  digitalWrite(LED_BUILTIN, val ? HIGH : LOW);
}

bool initEsp() {
  if (!sendCommand("AT", 5, "OK")) {
    return false;
  }

  if (!sendCommand("AT+CIPMUX=0", 5, "OK")) {
    return false;
  }

  if (!sendCommand("AT+CIPMODE=0", 5, "OK")) {
    return false;
  }

  return true;
}

bool resetEsp() {
  if (!sendCommand("AT+RST", 5, "ready")) {
    return false;
  }

  setErrorMode(false);
  return true;
}


bool connectToWifi(){
  if (!sendCommand("AT+CWJAP=\"" + Conf::wifiAccessName + "\",\"" + Conf::wifiPassword + "\"", 20, "OK")) {
    return false;
  }

  return true;
}

bool sendRequest(const String& request) {
  if (!sendCommand("AT+CIPSTART=\"TCP\",\"" + Conf::connectionHost + "\"," + Conf::connectionPort, 15, "OK")) {
    return false;
  }

  if (!sendCommand("AT+CIPSEND=" + String(request.length()), 4, ">")) {
    return false;
  }

  glob::Serial.println(request);

  if (!sendCommand("AT+CIPCLOSE", 5, "OK")) {
    return false;
  }

  return true;
}

bool reconnectToWifi() {
  if(!resetEsp()) {
    return false;
  }

  if (!initEsp()) {
    return false;
  }

  if (!connectToWifi()) {
    return false;
  }

  return true;
}

void setup() {
  glob::Serial.begin(115200);
  glob::Dht.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  reconnectToWifi();
}

void loop() {
  const float temp = glob::Dht.readTemperature(false, true);
  if (isnan(temp)) {
    setErrorMode(true);
    delay(100);
    deepSleepFor8s();
    return;
  }

  const String request = "GET " + Conf::webServerRequestUrl + String(temp) + "°C HTTP/1.1\r\nHost: " + Conf::connectionHost + "\r\n\r\n";

  if (!sendRequest(request)) {
    if (reconnectToWifi()) {
      sendRequest(request);
    }
  }

  delay(100);
  deepSleepFor8s(112);
}
