#ifndef DEFAULT_CONF_HPP
#define DEFAULT_CONF_HPP

#include <DHT.h>

struct Conf
{
    static inline const char* wifiAccessName = "YOUR_ACCESS_NAME";
    static inline const char* wifiPassword = "YOUR_PASSWORD";

    static inline const char* connectionHost = "YOUR_HOST";
    static inline const char* connectionPort = "YOUR_PORT";

    static inline const char* webServerRequestUrl = "/api/rooms/<int:room_id>/temps";

    static inline const int espPowerPin = 7;

    static inline const int dhtPin = 4;
    static inline const int dhtType = DHT22;
};

#endif // DEFAULT_CONF_HPP
