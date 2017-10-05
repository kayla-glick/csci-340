
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2017
// server.h header file
// Homework 1
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

// Function Definition
void handle_response(int client_socket_fd);
void build_params(char dest[], char* token);

// ------------------------------------
// Function prototype that creates a socket and 
// then binds it to the specified port_number 
// for incoming client connections
// 
//
// Arguments:	port_number = port number the server 
//				socket will be bound to.
//
// Return:      Socket file descriptor (or -1 on failure)
//

int bind_port( unsigned int port_number ) {

	// -------------------------------------
	// NOTHING TODO HERE :)
	// -------------------------------------
	// Please do not modify

	int socket_fd;
	int set_option = 1;

    struct sockaddr_in server_address;
     
    socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

    setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set_option, sizeof( set_option ) );

    if (socket_fd < 0) return FAIL;

    bzero( (char *) &server_address, sizeof(server_address) );

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons( port_number );

    if ( bind( socket_fd, (struct sockaddr *) &server_address, sizeof(server_address) ) == 0 ) {

    	return socket_fd;

    } else {

    	return FAIL;

    }

} // end bind_port function

// ------------------------------------
// Function prototype that accepts a client
// socket connection
// 
//
// Arguments:	server file descriptor
//
// Return:      Termination status of client
//				( 0 = No Errors, -1 = Error )
//
int accept_client( int server_socket_fd ) {

	int exit_status = OK;

	int client_socket_fd = -1;

	socklen_t client_length; 

	struct sockaddr_in client_address;

	client_length = sizeof( client_address );

  client_socket_fd = accept( server_socket_fd, (struct sockaddr *) &client_address, &client_length );
		
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Modify code to fork a child process
	// -------------------------------------
	
	int pid = fork();
		
	if ( pid > 0) {
		// Parent Process
		close( client_socket_fd );
	} else if ( pid == 0 ) {
		// Child Process
		if ( client_socket_fd >= 0 ) {
			close( server_socket_fd );
			handle_response(client_socket_fd);
			close( client_socket_fd );
		} else {
			exit_status = FAIL;
		}
	} else {
		printf("Failed to fork child process. Exiting...");
		exit_status = FAIL;
	}		
	
	if ( DEBUG ) printf("Exit status = %d\n", exit_status );
	
	return exit_status;
	
} // end accept_client function

// ------------------------------------
// Function prototype that handles the response
// from the forked child process
// 
//
// Arguments:	client file descriptor
//
// Return: void
//
void handle_response(int client_socket_fd) {
	char request[512];

	bzero( request, 512 );
	
	read( client_socket_fd, request, 511 );
	
	if ( DEBUG ) printf("Here is the http message:\n%s\n\n", request );
	
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Generate the correct http response when a GET or POST method is sent
	// from the client to the server.
	// 
	// In general, you will parse the request character array to:
	// 1) Determine if a GET or POST method was used
	// 2) Then retrieve the key/value pairs (see below)
	// -------------------------------------
	
	char entity_body[256] = "<html><body><h2>CSCI 340 (Operating Systems) Project 1</h2><table border=1 width=\"50%\"><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>";
	
	char* token = strtok(request, " ");

	if ( strcmp(token, "GET") == 0 ) {
		/*
		 ------------------------------------------------------
		 GET method key/values are located in the URL of the request message
		 ? - indicates the beginning of the key/value pairs in the URL
		 & - is used to separate multiple key/value pairs 
		 = - is used to separate the key and value
		 
		 Example:
		 
		 http://localhost/?first=brent&last=munsell
		 
		 two &'s indicated two key/value pairs (first=brent and last=munsell)
		 key = first, value = brent
		 key = last, value = munsell
		 ------------------------------------------------------
		 */

		token = strtok(NULL, " ") + 1;
		if ( token[0] == '?' ) token++;

		build_params(entity_body, token);
	} else if ( strcmp(token, "POST") == 0 ) {
		printf("Processing POST request...\n");
		/*
		 ------------------------------------------------------
		 POST method key/value pairs are located in the entity body of the request message
		 ? - indicates the beginning of the key/value pairs
		 & - is used to delimit multiple key/value pairs 
		 = - is used to delimit key and value
		 
		 Example:
		 
		 first=brent&last=munsell
		 
		 two &'s indicated two key/value pairs (first=brent and last=munsell)
		 key = first, value = brent
		 key = last, value = munsell
		 ------------------------------------------------------
		 */
		
		char* tok_ptr;
		while( (tok_ptr = strtok(NULL, "\r\n")) != NULL) {
			token = tok_ptr;
		}

		build_params(entity_body, token);
	} else {
		// We would expect this not to happen, but it's a precaution
		printf("Request used an invalid method. Valid methods are GET and POST...");
	}
	
	strcat(entity_body, "</tbody></table></body></html>");
	
	char response[512];
	sprintf( response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", (int)strlen( entity_body ), entity_body );

	if ( DEBUG ) printf( "%s\n", response );
	
  write( client_socket_fd, response, strlen( response ) );
} // end handle_response function

void build_params(char dest[], char* token) {
	char* param_token;
	char* end_param;
	char* val_token;
	param_token = strtok_r(token, "&", &end_param);
	while ( param_token != NULL) {
		char* end_val;
		strcat(dest, "<tr>");
		val_token = strtok_r(param_token, "=", &end_val);
		sprintf(&dest[strlen(dest)], "<td>%s</td>", val_token);
		val_token = strtok_r(NULL, "=", &end_val);
		sprintf(&dest[strlen(dest)], "<td>%s</td>", val_token);
		strcat(dest, "</tr>");
		param_token = strtok_r(NULL, "&", &end_param);
	}
}

