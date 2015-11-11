#include <stdlib.h>
#include <stdio.h>

#include "file.h"

int main() {
	file_t * f = create_file();
	char * tmp;
	push_file(f, "benoit");
	push_file(f, "bleues");
	tmp = top_file(f);
	printf("%s", tmp);
	free(tmp);
	delete_file(f);
	return 0;
}
