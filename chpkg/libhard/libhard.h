#ifndef LIBHARD_H
#define LIBHARD_H

extern bool hard_is_hard(char* filename);
extern void hard_split_pkg(FILE* package, FILE* metadatam, FILE* payload);
extern int  hard_decrypt_package(char* package, char* decrypted_file);
extern bool hard_is_package_encrypted(FILE* package);

#endif
