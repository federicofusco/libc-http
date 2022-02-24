#include <stdio.h>
#include <stdlib.h>

#include "http.h"

int main ( int argc, char* argv[] ) {

	send_http_request ( argv[1] );

	exit ( EXIT_SUCCESS );

}