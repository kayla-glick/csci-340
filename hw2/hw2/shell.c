// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist, however in future, more could 
// be added to the builtin command array.
// --------------------------------------------
const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {

	// ----------------------------------------
	// TODO: you fully implement this function
	char tokens[255][255];	
	
	char* token = strtok(line, " ");
	
	int i = 0;
	while( token != NULL ) {
		strcpy(tokens[i], token);
		i++;
		token = strtok(NULL, " ");  
	}
	
	if ( i <= 0 ) {
		p_cmd -> argc = 0;
	} else if ( find_fullpath(tokens[0], p_cmd) == 0 ) {
		p_cmd -> argc = ERROR;
	} else {
		p_cmd -> argc = i;
		
		int j = 0;
		while ( j < i ) {
			p_cmd -> argv[j] = tokens[j];				
			j++;
		}
	}
} // end parse function


int execute( command_t* p_cmd ) {
	// ----------------------------------------
	// TODO: you fully implement this function
	
	int status = ERROR;
	
	if ( p_cmd -> argc > 0 ) {
		if ( fork() == 0 ) {
			p_cmd -> argv[p_cmd -> argc] = NULL;
			execv(p_cmd -> path, p_cmd -> argv);
			exit(0);			
		}
		wait(0);
		status = SUCCESSFUL;
	} else {
		printf("Command not found...");
	}

	return status;


} // end execute function


int find_fullpath( char* command_name, command_t* p_cmd ) {
	// ----------------------------------------
	// TODO: you fully implement this function
	int found;

	strcpy(p_cmd -> path, command_name);

	if ( is_builtin(p_cmd) ) {
		found = TRUE;
	} else {	
		char *token;
		char path[255];
		char full_path[255];
		int exists;

		struct stat buffer;

		strcpy(path, getenv( "PATH" ));

		found = FALSE;	
		token = strtok(path, ":");
	
		while ( token != NULL ) {
			sprintf(full_path, "%s/%s", token, command_name);	
			exists = stat(full_path, &buffer);
		
			if ( exists == 0 && ( S_IFDIR & buffer.st_mode ) ) {
				// Directory Exists
				strcpy(p_cmd -> path, full_path);
				found = TRUE;
				break;
			} else if ( exists == 0 && ( S_IFREG & buffer.st_mode ) ) {
				// File Exists
				strcpy(p_cmd -> path, full_path);
				found = TRUE;
				break;
			} else {
				// Not a valid file or directory
				strcpy(p_cmd -> path, command_name);
			}

			token = strtok(NULL, ":");  
		}
	}

	return found;

} // end find_fullpath function


int is_builtin( command_t* p_cmd ) {

	int cnt = 0;

	while ( valid_builtin_commands[cnt] != NULL ) {

		if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

			return TRUE;

		}

		cnt++;

	}

	return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

	// only builtin command is cd

	if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

	struct stat buff;
	int status = ERROR;

	if ( p_cmd->argc == 1 ) {

		// -----------------------
		// cd with no arg
		// -----------------------
		// change working directory to that
		// specified in HOME environmental 
		// variable

		status = chdir( getenv("HOME") );

	} else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


		// -----------------------
		// cd with one arg 
		// -----------------------
		// only perform this operation if the requested
		// folder exists

		status = chdir( p_cmd->argv[1] );

	} 

	return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

	int i=0;
	
	while ( p_cmd->argv[i] != NULL ) {
		free( p_cmd->argv[i] );
		i++;
	}

	free( p_cmd->argv );
	free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {

	// First check length

	int len[] = {0,0};

	char* b_str1 = str1;
	const char* b_str2 = str2;

	while( (*str1) != '\0' ) { 
		len[0]++;
		str1++;
	}

	while( (*str2) != '\0' ) {
		len[1]++;
		str2++;
	}

	if ( len[0] != len[1] ) {

		return FALSE;

	} else {

		while ( (*b_str1) != '\0' ) {

			// don't care about case (you did not have to perform
			// this operation in your solution

			if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

				return FALSE;

			}

			b_str1++;
			b_str2++;

		}

	} 

	return TRUE;


} // end compare function definition

