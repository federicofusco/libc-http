#ifndef HTTP_H
#define HTTP_H

#ifndef HTTP_USER_AGENT
#define HTTP_USER_AGENT "libc-http (Linux)"
#endif

#include "http_response.h"

http_response send_http_request ( char* url );

#endif