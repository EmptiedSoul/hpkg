#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

/* We assume package is correct and isnot corrupted
 * so if mandatory PKGNAME field is not found on line 2 it means that package is encrypted
 * and we will need to call gpg later.
 */

bool hard_is_package_encrypted(FILE* package){
	char* string = NULL;
	size_t len = 0;
	bool encrypted;

	(void) getline(&string, &len, package); /* skipping 1 line */
	free(string);

	string = NULL;

	(void) getline(&string, &len, package); /* 2 second line - should be PKGNAME="..." */
	
	if (strstr(string, "PKGNAME") != NULL) 
		encrypted = false;
	else
		encrypted = true; /* Assuming package is correct */

	free(string);
	rewind(package); /* Return stream pointer to initial position */

	return encrypted;
}
