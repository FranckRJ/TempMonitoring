#ifndef CONF_HPP
#define CONF_HPP

struct DefaultConf
{
  static inline const String WifiAccessName = "YOUR_ACCESS_NAME";
  static inline const String WifiPassword = "YOUR_PASSWORD";

  static inline const String ConnectionHost = "YOUR_HOST";
  static inline const String ConnectionPort = "YOUR_PORT";

  static inline const String WebServerRequestUrl = "YOUR_REQUEST_URL";

  static inline const int DhtPin = 4;
};

# if __has_include("private.conf.hpp")
#  include "private.conf.hpp"
# else

struct Conf : public DefaultConf
{
};

# endif // __has_include("private.conf.hpp")

#endif // CONF_HPP
