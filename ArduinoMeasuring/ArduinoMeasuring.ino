#include <SoftwareSerial.h>
#include <DHT.h>
#include <LowPower.h>

#include "Conf.hpp"
#include "EspAtCmdWrapper.hpp"

namespace glob
{
    DHT dht{Conf::dhtPin, Conf::dhtType};
    EspAtCmdWrapper espAtCmdWrapper{LED_BUILTIN, Serial, Conf::wifiAccessName, Conf::wifiPassword};
} // namespace glob

void deepSleepFor8s(int times = 1)
{
    int oldClkPr = CLKPR;
    CLKPR = 0x80;
    CLKPR = 0x08;
    for (int i = 0; i < times; ++i)
    {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
    CLKPR = 0x80;
    CLKPR = oldClkPr;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    glob::dht.begin();
    glob::espAtCmdWrapper.begin();
}

void loop()
{
    const float temp = glob::dht.readTemperature(false, true);
    if (isnan(temp))
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        deepSleepFor8s();
        return;
    }

    const String form = "value=" + String(temp);

    String request;
    request.concat("POST ");
    request.concat(Conf::webServerRequestUrl);
    request.concat(" HTTP/1.1\r\n");
    request.concat("Host: ");
    request.concat(Conf::connectionHost);
    request.concat("\r\n");
    request.concat("Content-Type: application/x-www-form-urlencoded\r\n");
    request.concat("Content-Length: ");
    request.concat(String(form.length()));
    request.concat("\r\n\r\n");
    request.concat(form);

    glob::espAtCmdWrapper.sendRequestWithResetIfFail(request, Conf::connectionHost, Conf::connectionPort, 2);

    delay(100);
    deepSleepFor8s(112);
}
