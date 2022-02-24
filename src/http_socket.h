#ifndef HTTP_SOCKET_H
#define HTTP_SOCKET_H

#ifndef HTTP_TIME_OUT
#define HTTP_TIME_OUT 10
#endif

#ifndef DEFAULT_RESPONSE_SIZE
#define DEFAULT_RESPONSE_SIZE 128
#endif 

#include "http_url.h"
#include "http_response.h"
#include "http_request.h"

int create_http_connection ( http_url* url );
int http_send ( int descriptor, char* data );
int http_read ( int descriptor, http_request* data );
int http_await_response ( int descriptor, http_request* data );
http_response http_recieve ( int descriptor, http_request* data );

#endif