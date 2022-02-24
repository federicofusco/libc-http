#include <stdlib.h>
#include <string.h>

#include "http_request.h"

/**
 * Creates a new http_request struct,
 * destroy_http_request () must be called at the end of the 
 * program to free the request
 *
 * @returns An empty request structure
 */
http_request* create_http_request () {

	// Creates a new http request
	http_request* request = calloc ( 1, sizeof ( http_request ) );
	request -> state = calloc ( 1, sizeof ( http_state ) );
	request -> header = calloc ( 1,  sizeof ( http_header ) );
	return request;
}

/**
 * Frees a given request struct
 *
 * @param request - The request which should be freed
 * @returns Whether or not the struct was deallocated
 */
int destroy_http_request ( http_request* request ) {

	// Frees the request
	free ( request -> state );
	free ( request -> header );
	free ( request );
	return 1;
}