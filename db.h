#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define DBOBJ_PRINT(o) dbobj_print((o), #o)
#define STRIDE_SIZE(o) sizeof((o).strides) / sizeof(size_t)

#define MAX_NAME 69

// Error codes
#define ERR_EXISTS 24
#define ERR_OPENFILE 25
#define ERR_READ 26
#define ERR_WRITE 27
#define ERR_OBJ_NOT_FOUND 64

typedef struct {
	size_t n;
	size_t s;
	size_t *strides;
	char name[MAX_NAME];
	char *contents;
} DBObj;

DBObj dbobj_alloc(char name[], size_t n) {
	DBObj o;

	o.strides = malloc(sizeof(size_t) * 1);
	o.strides[0] = 0;
	o.s += sizeof(size_t);
	// o.name = name;
	memcpy(o.name, name, MAX_NAME);
	o.n = n;
	o.contents = malloc(n);
	o.contents[n] = 0x02; // magic hex code for marking the end of the Database Object
	
	return o;
}

void dbobj_print(DBObj o, const char *name) {
	printf("%s = { \n", name);
	printf("    name = %s\n", o.name);
	printf("    contents = <(- \" %s \" -)>\n", o.contents);
	printf("}\n\n");
}

void write_content(DBObj o, char *src, size_t n, size_t index) {
	o.strides = realloc(o.strides, sizeof(size_t) * STRIDE_SIZE(o));
	o.strides[STRIDE_SIZE(o)] = index;
	o.s += sizeof(size_t);

	for (size_t i = 0; i < n; ++i) {
		o.contents[index + i] = src[i];
	}
	o.contents[index + n] = 0x1A; // magic hex code for marking the end of contents
}

void read_content(DBObj o, size_t index, char *dst) {
	// The index should be equal to (exact position of "0xA1" character) + 1
	assert(o.contents[index - 1] == 0xA1);
	size_t i = 0;
	do {
		dst[i] = o.contents[index + i];
		i++;
	} while (o.contents[index + 1] != 0x1A || o.contents[index + 1] != 0x02); // check for end of contents or end of DBObj
}


//////////////// Goodbye, Dynamic memory allocation! /////////////////////


//// TODO: add function for setting up file signature for our database, and check for it in save_to_db()
//int setup_db(char *path) {
//}

int save_db(DBObj o, char *filename) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		perror("Can't write to local storage: Couldn't open database file");
		return ERR_OPENFILE;
	}

	// Set the file pointer to the location after the first four signature bytes
	DBObj read_struct;
	size_t next_obj_offset = 0;

	// Check if structure with the specified name already exists
	while (1) {
		size_t elements_read = fread(&read_struct, sizeof(DBObj), 1, file);
		if (elements_read != 1) {
			fprintf(stderr, "Read error\n");
			return ERR_READ;
		}
		
		if (strcmp(o.name, read_struct.name) == 0) {
			fprintf(stderr, "Structure with this name already exists.\n");
			fclose(file);
			return ERR_EXISTS;
		}

		next_obj_offset += read_struct.n + sizeof(DBObj);

		if (next_obj_offset > ftell(file)) break;

		fseek(file, next_obj_offset, SEEK_SET);
	}
	// Reopen the file for writing
	fclose(file);
	FILE *new_file = fopen(filename, "wb");

	fseek(new_file, 0, SEEK_END);

	// Write the Database Object, the strides and content to the local database
	fwrite(&o, sizeof(DBObj), 1, new_file);
	fwrite(o.strides, sizeof(o.strides), (sizeof(o.strides)/sizeof(size_t)), new_file);
	fwrite(o.contents, sizeof(o.contents), sizeof(o.contents), new_file);

	fclose(new_file);

	printf("Object saved to database successfully.\n");

	return 0;	
}

/////////////// Hello, Again :( //////////////////


// Read database and retreive object by name (thats what the n stands for in nread_db)
// Next we're gonna create a function that reads the database into a list of dynamically allocated DBObjs (lread_db)
//int nread_db(DBObj o, char *filename, char *obj_name) {
//
//}

// Can your SQL do THAT? I don't freaking think so XD //
