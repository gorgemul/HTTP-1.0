#include "../include/client.h"
#include <string.h>
#include <stdio.h>

int validate_input(int argc, const char **argv)
{
	if ((argc != 3) || (strlen(argv[2]) != 1))
		return -1;

	switch(*argv[2]) {
        case 'y':
        case '1':
                return 1;
        case 'n':
        case '0':
        	return 0;
        default:
                return -1;
	}
}

int parse_url(const char *url, struct UrlInfo *ui)
{
        switch (sscanf(url, "http://%31[^/]/%63[^\n]", ui->host, ui->path)) {
        case 2:
                return (strlen(ui->host) && strlen(ui->path)) ? 0 : -1;
        case 1:
                return -1;
        default:
                return -1;
        }

}
