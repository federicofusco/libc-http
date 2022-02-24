#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "http_url.h"

typedef struct {
	int code;
	int length;
} http_header;

typedef struct {
	int in_content;
	int chunk;
	char buffer[4096];
	int size;
	char* offset;
	char* last;
	int free;
	int left;
	int total;
} http_state;

typedef struct {
	http_header* header;
	http_state* state;
	char* content;
	int length;
} http_request;

http_request* create_http_request ();
int destroy_http_request ( http_request* request );

#endif