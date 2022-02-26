#include <stdio.h>
#include <stdlib.h>

// #include "http/http.h"
#include "crypto/rand/rand.h"

int main (/* int argc, char* argv[] */) {

	// if ( argc < 2 ) {

	// 	printf ( "Error: No URL was given!\n" );
	// 	exit ( EXIT_FAILURE );
	// }

	// http_response* response = send_http_request ( argv[1] );
	// destroy_http_response ( response );

	char* bytes = urandom_bytes ( 10 );
	for ( int x = 0; x < 10; x++ ) {
		printf ( "%d ", *(bytes + x ) );
	}
	printf ( "\n" );

	exit ( EXIT_SUCCESS );

}