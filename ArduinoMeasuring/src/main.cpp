#include <Arduino.h>
#include <DHT.h>
#undef min // Pourquoi Arduino, POURQUOI ?????
#undef max // :'(
#include <etl/circular_buffer.h>

#include "Conf.hpp"
#include "EspAtCmdWrapper.hpp"
#include "HttpRequestBuilder.hpp"
#include "LoopScheduler.hpp"

constexpr LoopScheduler::TimeType loopCycleDurationMs = 900'000UL;

namespace glob
{
    DHT dht{Conf::dhtPin, Conf::dhtType};
    EspAtCmdWrapper espAtCmdWrapper{Conf::espPowerPin, LED_BUILTIN, Serial, Conf::wifiAccessName, Conf::wifiPassword};
    etl::circular_buffer<float, 100> tempBuffer;
    LoopScheduler loopScheduler{loopCycleDurationMs};
} // namespace glob

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

    Serial.begin(115'200);
    glob::dht.begin();
    glob::espAtCmdWrapper.goToConnectedState();
    glob::loopScheduler.init();
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

                const auto delay
                    = static_cast<int32_t>((loopCycleDurationMs * glob::tempBuffer.size())
                                           + (500 + glob::loopScheduler.currentLoopCycleDuration()) / 1'000);

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
    glob::loopScheduler.waitForEndOfLoopCycle();
    glob::espAtCmdWrapper.goToConnectedState();
}
