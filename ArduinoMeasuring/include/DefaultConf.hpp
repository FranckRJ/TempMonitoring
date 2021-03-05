#ifndef DEFAULT_CONF_HPP
#define DEFAULT_CONF_HPP

#include <DHT.h>

struct Conf
{
    static inline const String wifiAccessName = "YOUR_ACCESS_NAME";
    static inline const String wifiPassword = "YOUR_PASSWORD";

    static inline const String connectionHost = "YOUR_HOST";
    static inline const String connectionPort = "YOUR_PORT";

    static inline const String webServerRequestUrl = "/api/rooms/<int:room_id>/temps";

    static inline const int espPowerPin = 7;

    static inline const int dhtPin = 4;
    static inline const int dhtType = DHT22;
};

#endif // DEFAULT_CONF_HPP
