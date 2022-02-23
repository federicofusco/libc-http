#ifndef HTTP_URL_H
#define HTTP_URL_H

#ifndef SUPPORTED_URL_SCHEMES
#define SUPPORTED_URL_SCHEMES "http"
#endif


typedef struct {
	char* scheme;
	char* userinfo;
	char* host;
	char* path;
	char* query;
	char* fragment;
} http_url;

http_url* create_http_url ( char* url_addr );
http_url* parse_http_url ( http_url* url, char* url_addr );
int destroy_http_url ( http_url* url );

#endif