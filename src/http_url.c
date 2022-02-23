#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_url.h"

/**
 * Creates an empty URL struct and parses a given URL
 *
 * @param url_addr - The URL which should be parsed
 * @returns A struct containing a parsed URL
 */
http_url* create_http_url ( char* url_addr ) {

	// Creates and parses the URL
	http_url* url = calloc ( 1, sizeof ( http_url ) + strlen ( url_addr ) + 1 );
	url = parse_url ( url, url_addr );

	return url;
}

/**
 * Parses a given URL
 *
 * @param url      - The URL struct where the parsed URL should be stored
 * @param url_addr - The URL address (which should be parsed)
 * @returns A struct with a parsed URL
 */
http_url* parse_http_url ( http_url* url, char* url_addr ) {

	size_t url_length = strlen ( url_addr );
	char* url_buffer;
	char* url_pointer;
	char* original_buffer;

	// // Creates an empty URL
	// http_url* url = calloc ( 1, sizeof ( http_url ) + url_length + 1 );

	// Copies the URL to the url buffer
	url_buffer = (char*) url + sizeof ( http_url );
	if ( !url_buffer ) {

		// Failed to allocate memory to the url buffer
		printf ( "ERROR: Failed to allocate memory to the url buffer!\n" );
		exit ( EXIT_FAILURE );
	}
	memcpy ( url_buffer, url_addr, url_length );
	original_buffer = url_buffer;

	// Gets the URL scheme
	// Note the only currently supported scheme is HTTP, HTTPS coming soon
	url_pointer = strstr ( url_buffer, "://" );
	if ( url_pointer ) {

		// Gets the URL scheme by adding a null terminator
		*( url_pointer ) = 0;
		url -> scheme = url_buffer;
		url_buffer = url_pointer + 3;

		// Checks if the scheme is supported
		if ( !strstr ( SUPPORTED_URL_SCHEMES, url -> scheme ) ) {

			// The scheme is not supported currently
			printf ( "ERROR: The scheme (%s) is currently not supported!\n", url -> scheme );
			exit ( EXIT_FAILURE );
		}
	} else {

		printf ( "ERROR: Invalid URL given, no scheme found!\n" );
		exit ( EXIT_FAILURE );
	}

	// Gets any userinfo from the given URL
	url_pointer = strchr ( url_buffer, '@' );
	if ( url_pointer ) {

		// Gets the userinfo by adding a null terminator
		*( url_pointer ) = 0;
		url -> userinfo = url_buffer;
		url_buffer = url_pointer + 1;
	}

	// Gets the URL host, path, query and fragments
	url_pointer = strchr ( url_buffer, '/' );
	if ( url_pointer ) {

		// Gets the URL host, knowing that a path is present
		*url_pointer = 0;
		url -> host = url_buffer;
		url_buffer = url_pointer + 1;

		// Checks if a query is present
		url_pointer = strchr ( url_buffer, '?' );
		if ( url_pointer ) {

			// A URL query is present
			// Gets the URL path
			*url_pointer = 0;
			url -> path = url_buffer;
			url_buffer = url_pointer + 1;

			// Checks if a fragment is present
			url_pointer = strchr ( url_buffer, '#' );
			if ( url_pointer ) {

				// A fragment is present
				// Gets the URL query
				*url_pointer = 0;
				url -> query = url_buffer;
				url_buffer = url_pointer + 1;

				// Gets the URL fragment
				url -> fragment = url_buffer;
			} else {

				// Get the URL query
				url -> query = url_buffer;
			}
		} else {

			// Checks if a fragment is present
			url_pointer = strchr ( url_buffer, '#' );
			if ( url_pointer ) {

				// A fragment is present
				// Gets the path
				*url_pointer = 0;
				url -> path = url_buffer;
				url_buffer = url_pointer + 1;

				// Gets the URL fragment
				url -> fragment = url_buffer;
			} else {

				// No fragment is present
				url -> path = url_buffer;
			}

		}
	} else {

		// There is no path, query or fragment
		url -> host = url_buffer;
	}

	return url;

}

/**
 * Frees a given URL struct
 *
 * @param url - The URL struct which should be freed
 */
int destroy_http_url ( http_url* url ) {

	// Frees all the memory from the URL struct
	free ( url );
	return 1;
}