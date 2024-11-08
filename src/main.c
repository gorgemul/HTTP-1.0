#include "../include/client.h"
#include <stdio.h>

int main(int argc, const char **argv)
{
	int is_saved = 0;
        struct UrlInfo ui;

	if ((is_saved = validate_input(argc, argv)) == -1) {
		fprintf(stderr, "USAGE: <program> <url> <'y'|'1'|'n'|'0'>");
		return 1;
	}

        return 0;
}
