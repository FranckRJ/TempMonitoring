#ifndef HTTP_REQUEST_BUILDER_HPP
#define HTTP_REQUEST_BUILDER_HPP

#include <WString.h>

class HttpRequestBuilder
{
public:
    static String postForm(const String& url, const String& host, const String& form);
};

#endif // HTTP_REQUEST_BUILDER_HPP
