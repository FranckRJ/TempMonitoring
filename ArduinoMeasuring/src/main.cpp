#include <Arduino.h>
#include <DHT.h>
#include <LowPower.h>
#undef min // Pourquoi Arduino, POURQUOI ?????
#undef max // :'(
#include <etl/circular_buffer.h>

#include "Conf.hpp"
#include "EspAtCmdWrapper.hpp"
#include "HttpRequestBuilder.hpp"

constexpr int pause8SecDuration = 112;
constexpr uint32_t pauseSecEstimateOfRealDuration = 976;

namespace glob
{
    DHT dht{Conf::dhtPin, Conf::dhtType};
    EspAtCmdWrapper espAtCmdWrapper{Conf::espPowerPin, LED_BUILTIN, Serial, Conf::wifiAccessName, Conf::wifiPassword};
    etl::circular_buffer<float, 100> tempBuffer;
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

float getTemp(int tries = 1)
{
    float temp = NAN;

    do
    {
        temp = glob::dht.readTemperature(false, true);

        if (!isnan(temp))
        {
            break;
        }

        --tries;
    } while (tries > 0);

    return temp;
}

bool sendTempRequestToEsp(float temp, int32_t delay = -1)
{
    String form = "value=";
    form.concat(temp);

    if (delay >= 0)
    {
        form.concat("&delay_in_sec=");
        form.concat(delay);
    }

    const String request = HttpRequestBuilder::postForm(Conf::webServerRequestUrl, Conf::connectionHost, form);

    return glob::espAtCmdWrapper.sendRequest(request, Conf::connectionHost, Conf::connectionPort);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    glob::dht.begin();
    glob::espAtCmdWrapper.goToConnectedState();
}

void loop()
{
    const float temp = getTemp(2);

    if (isnan(temp))
    {
        glob::tempBuffer.push(NAN);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        if (sendTempRequestToEsp(temp))
        {
            while (!glob::tempBuffer.empty())
            {
                const float oldTemp = glob::tempBuffer.front();

                if (isnan(oldTemp))
                {
                    glob::tempBuffer.pop();
                    continue;
                }

                const auto delay = static_cast<int32_t>(pauseSecEstimateOfRealDuration * glob::tempBuffer.size());

                if (sendTempRequestToEsp(oldTemp, delay))
                {
                    glob::tempBuffer.pop();
                }
                else
                {
                    glob::tempBuffer.push(NAN);
                    break;
                }
            }
        }
        else
        {
            glob::tempBuffer.push(temp);
        }
    }

    glob::espAtCmdWrapper.goToShutdownState();
    delay(100);
    deepSleepFor8s(pause8SecDuration);
    glob::espAtCmdWrapper.goToConnectedState();
}
