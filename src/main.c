#include <stdio.h>
#include <stdlib.h>

#include "http.h"

int main ( int argc, char* argv[] ) {

	if ( argc < 2 ) {

		printf ( "Error: No URL was given!\n" );
		exit ( EXIT_FAILURE );
	}

	http_response* response = send_http_request ( argv[1] );
	destroy_http_response ( response );

	exit ( EXIT_SUCCESS );

}