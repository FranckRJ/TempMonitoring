#include "EspAtCmdWrapper.hpp"

EspAtCmdWrapper::EspAtCmdWrapper(int errorLedPin, Stream& espSerial, const String& wifiAccessName, const String& wifiPassword)
    : _errorLedPin{errorLedPin}, _espSerial{espSerial}, _wifiAccessName{wifiAccessName}, _wifiPassword{wifiPassword}
{
}

bool EspAtCmdWrapper::begin()
{
    return reconnectToWifi();
}

bool EspAtCmdWrapper::sendRequest(const String& request, const String& host, const String& port)
{
    if (!sendCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, 15, "OK"))
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

bool EspAtCmdWrapper::sendRequestWithResetIfFail(const String& request, const String& host, const String& port, int maxTries = 1)
{
    for (int i = 0; i < maxTries; ++i)
    {
        if (sendRequest(request, host, port))
        {
            return true;
        }

        reconnectToWifi();
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
    if (!sendCommand("AT+CWJAP=\"" + _wifiAccessName + "\",\"" + _wifiPassword + "\"", 20, "OK"))
    {
        return false;
    }

    return true;
}

bool EspAtCmdWrapper::sendCommand(const String& command, int maxTries, char* expectedResult)
{
    int triesCount = 0;

    while (triesCount < maxTries)
    {
        _espSerial.println(command);
        if (_espSerial.find(expectedResult))
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
