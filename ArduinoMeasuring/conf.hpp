#ifndef CONF_HPP
#define CONF_HPP

#include <DHT.h>

struct DefaultConf
{
    static inline const String wifiAccessName = "YOUR_ACCESS_NAME";
    static inline const String wifiPassword = "YOUR_PASSWORD";

    static inline const String connectionHost = "YOUR_HOST";
    static inline const String connectionPort = "YOUR_PORT";

    static inline const String webServerRequestUrl = "YOUR_REQUEST_URL";

    static inline const int dhtPin = 4;
    static inline const int dhtType = DHT22;
};

#if __has_include("_Conf.hpp")
#include "_Conf.hpp"
#else

struct Conf : public DefaultConf
{
};

#endif // __has_include("_Conf.hpp")

#endif // CONF_HPP
