#include <stdio.h>
#include <stdlib.h>

#include "http_url.h"

int main ( int argc, char* argv[] ) {

	http_url* url = create_url ( argv[1] );
	destroy_url ( url );

	exit ( EXIT_SUCCESS );

}