#ifndef CLIENT_H
#define CLIENT_H

#define REQUEST_MESSAGE_MAX_SIZE 2048
#define RECV_MESSAGE_MAX_SIZE 204800

struct UrlInfo {
	char host[32];
	char path[64];
};

int validate_input(int argc, const char **argv);
int parse_url(const char *url, struct UrlInfo *ui);
void construct_request_message(char *req_msg, const char *uri);

/* Not test beacuse it needs the server running */
int connect_http_server(const char *host);

#endif
