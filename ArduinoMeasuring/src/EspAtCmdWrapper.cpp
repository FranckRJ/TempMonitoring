#include "EspAtCmdWrapper.hpp"

#include <Arduino.h>
#include <etl/utility.h>

EspAtCmdWrapper::EspAtCmdWrapper(
    int powerPin, int errorLedPin, Stream& espSerial, String wifiAccessName, String wifiPassword)
    : _powerPin{powerPin}
    , _errorLedPin{errorLedPin}
    , _espSerial{espSerial}
    , _wifiAccessName{etl::move(wifiAccessName)}
    , _wifiPassword{etl::move(wifiPassword)}
{}

bool EspAtCmdWrapper::begin()
{
    if (_powerPin >= 0)
    {
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, HIGH);
    }

    return reconnectToWifi();
}

bool EspAtCmdWrapper::sendRequest(const String& request, const String& host, const String& port)
{
    if (!sendCommand(R"(AT+CIPSTART="TCP",")" + host + "\"," + port, 10, "OK"))
    {
        return false;
    }

    if (!sendCommand("AT+CIPSEND=" + String(request.length()), 4, ">"))
    {
        return false;
    }

    _espSerial.println(request);

    if (!sendCommand("AT+CIPCLOSE", 5, "OK"))
    {
        return false;
    }

    return true;
}

bool EspAtCmdWrapper::sendRequestWithResetIfFail(const String& request,
                                                 const String& host,
                                                 const String& port,
                                                 int maxTries)
{
    for (int i = 0; i < maxTries; ++i)
    {
        if (sendRequest(request, host, port))
        {
            return true;
        }

        reconnectToWifi();
    }

    return false;
}

void EspAtCmdWrapper::shutdown()
{
    if (_powerPin >= 0)
    {
        digitalWrite(_powerPin, LOW);
    }
}

bool EspAtCmdWrapper::reconnectToWifi()
{
    if (!sendResetCmd())
    {
        return false;
    }

    if (!sendInitCmd())
    {
        return false;
    }

    if (!sendConnectToWifiCmd())
    {
        return false;
    }

    return true;
}

bool EspAtCmdWrapper::sendInitCmd()
{
    if (!sendCommand("AT", 5, "OK"))
    {
        return false;
    }

    if (!sendCommand("AT+CIPMUX=0", 5, "OK"))
    {
        return false;
    }

    if (!sendCommand("AT+CIPMODE=0", 5, "OK"))
    {
        return false;
    }

    return true;
}

bool EspAtCmdWrapper::sendResetCmd()
{
    if (!sendCommand("AT+RST", 5, "ready"))
    {
        return false;
    }

    return true;
}

bool EspAtCmdWrapper::sendConnectToWifiCmd()
{
    if (!sendCommand("AT+CWJAP=\"" + _wifiAccessName + "\",\"" + _wifiPassword + "\"", 15, "OK"))
    {
        return false;
    }

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
}
