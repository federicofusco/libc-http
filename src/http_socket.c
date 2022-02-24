#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#include "http_socket.h"
#include "http_url.h"
#include "http_response.h"
#include "http_request.h"

/**
 * Creates a new http connection
 *
 * @param url - A structure containing the parsed URL
 * @returns A socket descriptor, of -1 if an error occured
 */
int create_http_connection ( http_url* url ) {

	// Creates a new socket struct
	int socket_descriptor = -1;

	// Creates new address info linked lists
	struct addrinfo hints, *si, *pointer;
	memset ( &hints, 0, sizeof ( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// Gets the address info
	if ( getaddrinfo ( url -> host, url -> scheme, &hints, &si ) ) {

		// Failed to get address info
		printf ( "Error: Failed to get address!\n" );
		exit ( EXIT_FAILURE );
	}

	// Loops through all the address results, until a successfull
	// connection is established
	pointer = si;
	while ( pointer ) {

		// Attempts to create a connection descriptor
		socket_descriptor = socket ( pointer -> ai_family, pointer -> ai_socktype, pointer -> ai_protocol );
		if ( socket_descriptor < 0 ) {
			continue;
		}

		// Attempts to establish a connection using the socket descriptor
		if ( connect ( socket_descriptor, pointer -> ai_addr, pointer -> ai_addrlen ) < 0 ) {

			// Closes the connection
			close ( socket_descriptor );
			continue;
		}

		break;

	}

	// Checks if the connection was successfull
	if ( !( pointer ) && socket_descriptor > -1 ) {
		
		// The connection failed
		close ( socket_descriptor );
		socket_descriptor = -1;
	}

	// Deallocates the address info linked list
	freeaddrinfo ( si );
	return socket_descriptor;
}

/**
 * Sends bytes of data to a given socket descriptor
 *
 * @param descriptor - The socket descriptor
 * @param data       - The data which should be sent
 * @returns The number of bytes sent, -1 if an error occured
 */
int http_send ( int descriptor, char* data ) {
	
	// Gets the request size
	size_t len = strlen ( data );
	while ( len > 0 ) {
		
		// Sends the request to the socket
		int bytes = send ( descriptor, data, len, 0 );
		if ( bytes < 0 ) {
			return -1;
		}

		// Increments the offsets
		data += bytes;
		len -= bytes;
	}

	return 0;
}

/**
 * Reads data from a given socket descriptor and parsed the response
 * into an http_request struct
 *
 * @param descriptor - The socket descriptor
 * @param request    - Where the parsed reponse should be stored
 */
int http_read ( int descriptor, http_request* request ) {
	
	// Initializes the message state 
	if ( !request -> state -> offset ) {

		// Initializes the size of read buffer, 
		// add an extra byte for the null terminator ------------------v
		request -> state -> size = sizeof ( request -> state -> buffer ) - 1;
		request -> state -> offset = request -> state -> buffer;

		// Initializes the remaining length of chunk
		request -> state -> chunk = -1;

		// Initialize the content length
		request -> header -> length = -1;
	}

	// For keep-alive connections
	if ( request -> state -> total == request -> header -> length ) {
		return 0;
	}

	for ( ;; ) {

		// Attempt to parse the buffer if there's still data in it
		if ( request -> state -> left > 0 ) {

			// Parses the buffer
			char* parsed_until;
			request -> length = 0;
			parsed_until = parse_http_response ( request, request -> state -> offset, request -> state -> offset + request -> state -> left );
		
			if ( parsed_until > request -> state -> offset ) {

				// Updates the offsets
				request -> state -> left -= parsed_until - request -> state -> offset;
				request -> state -> offset = parsed_until;
				request -> state -> total += request -> length;

				return 1;
			}

		}

		// Resets the offset to point to the beginning of the state buffer
		if ( request -> state -> offset > request -> state -> buffer ) {

			if ( request -> state -> left > 0 ) {

				// Copies the offset to the beginning of the state buffer
				memmove ( request -> state -> buffer, request -> state -> offset, request -> state -> left );
			}

			request -> state -> offset = request -> state -> buffer;
		}

		// Read new data from the network
		{

			// Gets the buffer pointer and size
			char* append = request -> state -> offset + request -> state -> left;
			int size = ( request -> state -> buffer + request -> state -> size ) - append;

			// Drop half of the state buffer if it's too small
			if ( size < 1 ) {

				// Copies the last half of the state buffer to the beginning of the state buffer
				int half = request -> state -> size >> 1;
				memcpy ( request -> state -> buffer, request -> state -> buffer + half, half );

				// Updates the offset and left pointers
				request -> state -> offset = request -> state -> buffer;
				request -> state -> left = half;

				// Updates the append and size values
				append = request -> state -> offset + request -> state -> left;
				size = request -> state -> size - request -> state -> left;
			}

			// Recieves message and stores it in the last half of the state buffer
			int bytes_recieved = recv ( descriptor, append, size, 0 );
			if ( bytes_recieved < 1 ) {
				
				// Socket was closed
				return 0;
			}

			// Appends a null terminator at the end of the message
			append[bytes_recieved] = 0x00;
			request -> state -> left += bytes_recieved;
		}
	}

	return 0;
}

/**
 * Blocks until either data from is recieved from the socket descriptor or
 * the HTTP_TIME_OUT is reached, it then parses the response with http_read ()
 *
 * @param descriptor - The socket descriptor
 * @param request    - Where the parsed response should be stored   
 */
int http_await_response ( int descriptor, http_request* request ) {

	fd_set data_block;
	struct timeval timeout;

	// Sets the timeout
	timeout.tv_sec = HTTP_TIME_OUT;
	timeout.tv_usec = 0;

	FD_ZERO ( &data_block );
	FD_SET ( descriptor, &data_block );

	// Blocks until either a message is given or the timout is reached 
	if ( select ( descriptor + 1, &data_block, NULL, NULL, &timeout ) < 1 || !FD_ISSET ( descriptor, &data_block ) ) {
		return -1;
	}

	return http_read ( descriptor, request );
}

/**
 * Recieves data from a given socket descriptor and parses the response
 * into a response structure
 *
 * @param descriptor - The socket descriptor
 * @param request    - The request
 * @returns A parsed response structure
 */
http_response http_recieve ( int descriptor, http_request* data ) {

	// Creates a new empty HTTP response
	http_response* response = create_http_response ( DEFAULT_RESPONSE_SIZE );

	while ( http_await_response ( descriptor, data ) > 0 ) {
		if ( data -> content ) {

			// Appends the content to the response
			append_http_response ( response, data );
		}
	}

	return *response;
}