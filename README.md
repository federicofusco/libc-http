
# libc-http

A HTTP library I wrote in C. This does not currently support HTTPS, or any methods other than GET.



## Todo

 - Add support for other methods (e.i POST, DELETE, etc)
 - Support redirection

## Usage/Examples

Send an HTTP request
```c
#include "http.h"

int main () {

  // Creates a new HTTP response
  http_response* response;

  // Sends the response
  response = send_http_request ( "http://neverssl.com/" );

  // Remember to free the response when done
  destroy_http_response ( response );

  return 0;
}
```


## Environment Variables

`HTTP_TIME_OUT`

| Default | Description                        |
| :------ | :--------------------------------- |
| 10      | The timeout before a request fails |

`DEFAULT_REQUEST_SIZE`
| Default | Description                                          |
| :------ | :--------------------------------------------------- |
| 128     | This is the initial size of the of a response struct |

* If the actual response size is larger than `DEFAULT_REQUEST_SIZE`, new memory will be allocated to the response, but this can slow down the program depending on the actual response size


## Installation

You can install libc-http by copying `src/` (excluding `main.c` which isn't necessary) to your project source and include it like so:

```c
  #include "libc-http/http.c"
```
    
## License

[MIT](https://choosealicense.com/licenses/mit/)

