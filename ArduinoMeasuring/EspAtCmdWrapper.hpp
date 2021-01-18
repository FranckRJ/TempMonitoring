#ifndef ESP_AT_CMD_WRAPPER_HPP
#define ESP_AT_CMD_WRAPPER_HPP

#include <Arduino.h>

class EspAtCmdWrapper
{
public:
    explicit EspAtCmdWrapper(int errorLedPin, Stream& espSerial, const String& wifiAccessName, const String& wifiPassword);

    bool begin();

    bool sendRequest(const String& request, const String& host, const String& port);

    bool sendRequestWithResetIfFail(const String& request, const String& host, const String& port, int maxTries = 1);

private:
    bool reconnectToWifi();

    bool sendInitCmd();

    bool sendResetCmd();

    bool sendConnectToWifiCmd();

    bool sendCommand(const String& command, int maxTries, char* expectedResult);

    void setErrorMode(bool val);

private:
    int _errorLedPin;
    Stream& _espSerial;
    String _wifiAccessName;
    String _wifiPassword;
};

#endif // ESP_AT_CMD_WRAPPER_HPP
