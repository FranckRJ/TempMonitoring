#include "EspAtCmdWrapper.hpp"

#include <Arduino.h>
#include <etl/utility.h>

#define SEND_CMD_PROPAGATE_FAILURE(cmd, maxTries, expectedResult)                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!sendCommand(cmd, maxTries, expectedResult))                                                               \
        {                                                                                                              \
            return false;                                                                                              \
        }                                                                                                              \
    } while (false)

EspAtCmdWrapper::EspAtCmdWrapper(
    int powerPin, int errorLedPin, Stream& espSerial, String wifiAccessName, String wifiPassword)
    : _powerPin{powerPin}
    , _errorLedPin{errorLedPin}
    , _espSerial{espSerial}
    , _wifiAccessName{etl::move(wifiAccessName)}
    , _wifiPassword{etl::move(wifiPassword)}
{}

bool EspAtCmdWrapper::goToStartedState()
{
    if (_powerPin >= 0)
    {
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
    }

    delay(2500);
    _state = State::Started;

    return true;
}

bool EspAtCmdWrapper::goToConnectedState()
{
    switch (_state)
    {
        case State::Shutdown:
            if (!goToStartedState())
            {
                return false;
            }

            [[fallthrough]];
        case State::Started:
            if (!sendResetCmd())
            {
                return false;
            }

            if (!sendInitCmd())
            {
                return false;
            }

            [[fallthrough]];
        case State::Initialized:
            if (!sendConnectToWifiCmd())
            {
                return false;
            }

            [[fallthrough]];
        case State::Connected:
            break;
    }

    return true;
}

bool EspAtCmdWrapper::sendRequest(const String& request, const String& host, const String& port)
{
    if (!goToConnectedState())
    {
        return false;
    }

    SEND_CMD_PROPAGATE_FAILURE(R"(AT+CIPSTART="TCP",")" + host + "\"," + port, 10, "OK");

    SEND_CMD_PROPAGATE_FAILURE("AT+CIPSEND=" + String(request.length()), 4, ">");

    _espSerial.println(request);

    SEND_CMD_PROPAGATE_FAILURE("AT+CIPCLOSE", 5, "OK");

    return true;
}

void EspAtCmdWrapper::goToShutdownState()
{
    if (_powerPin >= 0)
    {
        digitalWrite(_powerPin, LOW);
    }

    _state = State::Shutdown;
}

bool EspAtCmdWrapper::sendInitCmd()
{
    SEND_CMD_PROPAGATE_FAILURE("AT", 5, "OK");

    SEND_CMD_PROPAGATE_FAILURE("AT+CIPMUX=0", 5, "OK");

    SEND_CMD_PROPAGATE_FAILURE("AT+CIPMODE=0", 5, "OK");

    _state = State::Initialized;

    return true;
}

bool EspAtCmdWrapper::sendResetCmd()
{
    SEND_CMD_PROPAGATE_FAILURE("AT+RST", 15, "ready");

    _state = State::Started;

    return true;
}

bool EspAtCmdWrapper::sendConnectToWifiCmd()
{
    SEND_CMD_PROPAGATE_FAILURE("AT+CWJAP=\"" + _wifiAccessName + "\",\"" + _wifiPassword + "\"", 15, "OK");

    _state = State::Connected;

    return true;
}

bool EspAtCmdWrapper::sendCommand(const String& command, int maxTries, const char* expectedResult)
{
    int triesCount = 0;

    while (triesCount < maxTries)
    {
        _espSerial.println(command);
        if (_espSerial.find(const_cast<char*>(expectedResult)))
        {
            setErrorMode(false);
            return true;
        }

        ++triesCount;
    }

    setErrorMode(true);
    return false;
}

void EspAtCmdWrapper::setErrorMode(bool val)
{
    if (_errorLedPin >= 0)
    {
        digitalWrite(_errorLedPin, val ? HIGH : LOW);
    }

    _state = State::Started;
}
