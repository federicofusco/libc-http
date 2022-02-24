#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "http.h"
#include "http_url.h"
#include "http_socket.h"
#include "http_request.h" 
#include "http_response.h"

/** 
 * Sends a request to the given url
 *
 * @param url_addr - The URL where the request should be sent
 * @returns A parsed response structure
 */ 
http_response* send_http_request ( char* url_addr ) {

	// Creates a new URL
	http_url* url = create_http_url ( url_addr );
	http_request* request = create_http_request ();

	// Creates a socket descriptor with a successul connection
	int socket_data = create_http_connection ( url );

	if ( !url || !socket_data ) {

		// Failed to establish connection
		printf ( "ERROR: Failed to connect to %s://%s/%s!\n", url -> scheme, url -> host, url -> path );
		exit ( EXIT_FAILURE );
	}

	if ( http_send ( socket_data, "GET /" ) ||
		 http_send ( socket_data, url -> path ) ||
		 http_send ( socket_data, " HTTP/1.1\r\nUser-Agent: "HTTP_USER_AGENT"\r\nHost: ") ||
		 http_send ( socket_data, url -> host ) ||
		 http_send ( socket_data, "\r\nAccept: */*\r\nConnection: close\r\n\r\n" ) ) {

		close ( socket_data );

		printf ( "Failed to connect to %s://%s/\n", url -> scheme, url -> host );
		exit ( EXIT_FAILURE );
	}

	http_response* response = http_await_response_data ( socket_data, request );

	// Frees memory
	destroy_http_url ( url );
	destroy_http_request ( request );

	printf ( "%s\n", response -> response );

	return response;

}