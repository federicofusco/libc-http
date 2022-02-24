#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stdio.h>

#include "http_url.h"
#include "http_request.h"

#ifndef DEFAULT_RESPONSE_SIZE
#define DEFAULT_RESPONSE_SIZE 128
#endif

#ifndef WHITESPACE
#define WHITESPACE " \t\r\n"
#endif

typedef struct {
	char* response;
	char* offset; 
	size_t size;
} http_response;

http_response* create_http_response ( size_t response_size );
int append_http_response ( http_response* response, http_request* request );
int destroy_http_response ( http_response* response );
http_response* http_await_response_data ( int descriptor, http_request* request );
void http_cut_trailing_crlf ( http_request* request );
char* parse_http_content ( http_request* request, char* body, char* body_end );
char* parse_http_response ( http_request* request, char* body, char* body_end );

#endif