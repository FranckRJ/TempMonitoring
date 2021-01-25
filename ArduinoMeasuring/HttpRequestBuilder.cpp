#include "HttpRequestBuilder.hpp"

String HttpRequestBuilder::postForm(const String& url, const String& host, const String& form)
{
    String request;

    request.concat("POST ");
    request.concat(url);
    request.concat(" HTTP/1.1\r\n"
                   "Host: ");
    request.concat(host);
    request.concat("\r\n"
                   "Content-Type: application/x-www-form-urlencoded\r\n"
                   "Content-Length: ");
    request.concat(String(form.length()));
    request.concat("\r\n\r\n");
    request.concat(form);

    return request;
}
