#include <stdlib.h>
#include <string.h>

#include "http_response.h"
#include "http_request.h"
#include "http_socket.h"
#include "http_url.h"

/**
 * Creates a new http_response struct in which a
 * response can be stored, destroy_http_response ()
 * must be called at the end of the program
 *
 * @param size - The size of the response body, this will change dynamically if needed,
 * 				 if the size is set to 0, the default size specified in http_response.h
 *				 will be used
 * @returns An empty http response struct
 */ 
http_response* create_http_response ( size_t size ) {

	if ( size == 0 ) 
		size = DEFAULT_RESPONSE_SIZE;

	// Creates an empty response
	http_response* response = malloc ( sizeof ( http_response ) );
	if ( !response ) {

		// Failed to allocate memory to the response
		printf ( "ERROR: Failed to allocate memory to response!\n" );
		exit ( EXIT_FAILURE );
	}

	response -> size = size;
	response -> response = calloc ( response -> size, 1 );
	if ( !response -> response ) {

		// Failed to allocate memory to the response body
		printf ( "ERROR: Failed to allocate memory to the response body!\n" );
		exit ( EXIT_FAILURE );
	}
	
	response -> offset = response -> response;

	return response;
}

/**
 * Appends the request content to the given response's body
 *
 * @param response - The structure containing the response
 * @param request  - The structure containing the request
 * @returns The number of bytes which were appended to the response
 */
int append_http_response ( http_response* response, http_request* request ) {

	// Checks if the response body is large enough
	if ( request -> length > ( response -> response + response -> size ) - response -> offset ) {

		// Reallocates the response body
		int offset = response -> offset - response -> response;
		response -> response = realloc ( response -> response, response -> size + request -> length );
		if ( ! ( response -> response ) ) {

			// Failed to allocate memory to the response body
			return -1;
		} 

		response -> size += request -> length;
		response -> offset = response -> response + offset;

	}

	// Appends the body
	strncat ( response -> response, request -> content, request -> length );
	response -> offset += request -> length;

	return request -> length;
}

/**
 * Destroys a given response structure
 *
 * @param response - The response structure which should be freed
 * @return Whether or not the response was deallocated
 */
int destroy_http_response ( http_response* response ) {

	// Frees the response struct
	free ( response -> response );
	free ( response );

	return 1;
}

/**
 * Given a descriptor and a request, blocks until data
 * is recieved and appends it to a new response structure
 *
 * @param descriptor - The socket descriptor
 * @param request    - The request structure
 * @returns A new structure containing a response
 */
http_response http_await_response_data ( int descriptor, http_request* request ) {

	// Creates a new empty HTTP response
	http_response* response = create_http_response ( DEFAULT_RESPONSE_SIZE );

	while ( http_await_response ( descriptor, request ) > 0 ) {
		if ( request -> content ) {

			// Appends the content to the response
			append_http_response ( response, request );
		}
	}

	return *response;
}

/**
 * Removes the trailing CR/LF from a given request
 * 
 * @param request - The request which should be trimmed
 */
void http_cut_trailing_crlf ( http_request* request ) {
	if ( request -> state -> chunk == 1 && request -> length > 0 ) {

		request -> content[request -> length--] = 0;
	} else if ( !request -> state -> chunk && request -> length > 1 ) {

		request -> length -= 2;
		request -> content[request -> length] = 0;
	} 
}

/**
 * Parses the content of a given request
 *
 * @param request  - The request which should be parsed
 * @param body     - A pointer to the beginning of the request body
 * @param body_end - A pointer to the ending of the request body
 * @returns The parsed body
 */
char* parse_http_content ( http_request* request, char* body, char* body_end ) {
	
	// Gets the body length
	size_t len = body_end - body;

	// Checks if the message is chunked
	if ( request -> state -> chunk > -1 ) {

		// Checks if this is the end of a chunk
		if ( request -> state -> chunk < len ) {
			
			char* chunk_end;

			if ( request -> state -> chunk > 0 ) {
				if ( request -> state -> chunk == 1 ) {
					
					// Skips LF from previous chunk header
					body++;
					request -> length = 0;
					request -> state -> chunk = 0;
				} else {
					
					// Return data before next chunk header
					request -> length = request -> state -> chunk;
					request -> content = body;

					body += request -> state -> chunk;

					request -> state -> chunk = 0;
					http_cut_trailing_crlf ( request );
				}

				return body;
			}

			// Checks that the chunk header is always at the beginning
			if ( ! ( chunk_end = strchr ( body, '\n' ) ) ) {
				
				// Chunk header is incomplete
				return body;
			}

			*chunk_end = 0;
			sscanf ( body, "%x", &request -> state -> chunk );

			// Adds trailing CR/LF to chunk size
			request -> state -> chunk += 2;

			request -> content = chunk_end++;
			len = body_end - request -> content;

			if ( request -> state -> chunk < len ) {
				
				// Chunk ends in this segment and a new one starts here
				request -> length = request -> state -> chunk;
				body = request -> content + request -> state -> chunk;

				// CR/LF must be together
				request -> state -> chunk = 0;
				http_cut_trailing_crlf ( request );

				return body;
			} else {
				/* next chunk header is beyond this segment */
				request -> length = len;

				request -> state -> chunk -= len;
				http_cut_trailing_crlf ( request );

				return body_end;
			}
		} else {
			/* next chunk header is beyond this segment;
			 * fall through to default behaviour */
			request -> state -> chunk -= len;
		}
	}

	request -> content = body;
	request -> length = len;

	http_cut_trailing_crlf ( request );

	return body_end;
}

/**
 * Parses the content of a given response
 *
 * @param request  - The response which should be parsed
 * @param body     - A pointer to the beginning of the response body
 * @param body_end - A pointer to the end of the response body
 * @returns The parsed body
 */
char* parse_http_response ( http_request* request, char* body, char* body_end ) {
	
	char* lf;

	// Parses message content
	if ( request -> state -> in_content ) {
		return parse_http_content ( request, body, body_end );
	}
	
	// If the header line is incomplete, fetch more data
	lf = strchr ( body, '\n' );
	if ( !lf ) {
		return body;
	}

	// Parse the HTTP status code
	if ( !request -> header -> code ) {

		// Accepts only HTTP/1.[01]
		if ( strncmp ( body, "HTTP/1.", 7 ) || !strchr ( "01", * ( body + 7 ) ) ) {
			return NULL;
		} 

		// Gets the HTTP status code
		body += 8;
		request -> header -> code = atoi ( body );
		
		return parse_http_response ( request, lf++, body_end );
	}

	// Parse header by line
	for ( ; lf; body = lf++, lf = strchr ( body, '\n' ) ) {

		char* value;
		*lf = 0;

		// Checks for the end of the header
		if ( !*body || !strcmp ( body, "\r" ) ) {

			// Increments the body pointer
			if ( !*body ) 
				body++;
			else 
				body += 2;

			// Updates the message state
			request -> state -> in_content = 1;
			return parse_http_response ( request, body, body_end );
		}

		value = strchr ( body, ':' );
		if ( !value ) {
			continue;
		}

		// Resets the value
		*value = 0;
		value++;

		body += strspn ( body, WHITESPACE );
		strtok ( body, WHITESPACE );

		value += strspn ( value, WHITESPACE );
		strtok ( value, WHITESPACE );

		if ( !strcasecmp ( body, "Transfer-Encoding" ) && !strcasecmp ( value, "chunked" ) ) {

			// 0 bytes until next header
			request -> state -> chunk = 0;
		} else if ( !strcasecmp ( body, "Content-Length" ) ) {
			request -> header -> length = atoi ( value );
		}

	}

	return body;
}