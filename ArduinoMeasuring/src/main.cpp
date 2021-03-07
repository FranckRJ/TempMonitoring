#include <Arduino.h>
#include <DHT.h>
#include <LowPower.h>

#include "Conf.hpp"
#include "EspAtCmdWrapper.hpp"
#include "HttpRequestBuilder.hpp"

namespace glob
{
    DHT dht{Conf::dhtPin, Conf::dhtType};
    EspAtCmdWrapper espAtCmdWrapper{Conf::espPowerPin, LED_BUILTIN, Serial, Conf::wifiAccessName, Conf::wifiPassword};
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
    delay(2500);
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
    const String request = HttpRequestBuilder::postForm(Conf::webServerRequestUrl, Conf::connectionHost, form);

    glob::espAtCmdWrapper.sendRequestWithResetIfFail(request, Conf::connectionHost, Conf::connectionPort, 2);

    glob::espAtCmdWrapper.shutdown();
    delay(100);
    deepSleepFor8s(112);
    glob::espAtCmdWrapper.begin();
}
