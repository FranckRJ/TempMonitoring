#ifndef ESP_AT_CMD_WRAPPER_HPP
#define ESP_AT_CMD_WRAPPER_HPP

#include <Stream.h>
#include <WString.h>

class EspAtCmdWrapper
{
public:
    explicit EspAtCmdWrapper(
        int powerPin, int errorLedPin, Stream& espSerial, String wifiAccessName, String wifiPassword);

    bool begin();

    bool sendRequest(const String& request, const String& host, const String& port);

    bool sendRequestWithResetIfFail(const String& request, const String& host, const String& port, int maxTries = 1);

    void shutdown();

private:
    bool reconnectToWifi();

    bool sendInitCmd();

    bool sendResetCmd();

    bool sendConnectToWifiCmd();

    bool sendCommand(const String& command, int maxTries, const char* expectedResult);

    void setErrorMode(bool val);

private:
    const int _powerPin;
    const int _errorLedPin;
    Stream& _espSerial;
    const String _wifiAccessName;
    const String _wifiPassword;
};

#endif // ESP_AT_CMD_WRAPPER_HPP
