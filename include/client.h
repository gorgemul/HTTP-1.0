#ifndef CLIENT_H
#define CLIENT_H

struct UrlInfo {
	char host[32];
	char path[64];
};

int validate_input(int argc, const char **argv);
int parse_url(const char *url, struct UrlInfo *ui);

#endif
