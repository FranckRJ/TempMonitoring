#ifndef CONF_HPP
#define CONF_HPP

#if __has_include("PrivateConf.hpp")
#include "PrivateConf.hpp"
#else // __has_include("PrivateConf.hpp")
#include "DefaultConf.hpp."
#endif // __has_include("PrivateConf.hpp")

#endif // CONF_HPP
