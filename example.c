#include <stdio.h>
#include "db.h"

int main() {
 
	char name[] = "test_name";
	DBObj o = dbobj_alloc(name, 256);

	write_content(o, "Hello World!", 12, 0);

	DBOBJ_PRINT(o);

	save_db(o, "database.db");

	return 0;
}
