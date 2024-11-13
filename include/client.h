#ifndef CLIENT_H
#define CLIENT_H

#define REQUEST_MESSAGE_MAX_SIZE 2048
#define RECV_MESSAGE_MAX_SIZE 204800
#define CONTENT_MAX_SIZE 8192
#define TOTAL_STATUS 15
#define VERSION_MAX_LENGTH 9
#define CODE_MAX_LENGTH 4
#define MESSAGE_MAX_LENGTH 30

struct UrlInfo {
	char host[32];
	char path[64];
};

struct HttpResponse {
    /* Initial line */
    char http_version[VERSION_MAX_LENGTH];
    char status_code[CODE_MAX_LENGTH];
    char status_message[MESSAGE_MAX_LENGTH];

    char content[CONTENT_MAX_SIZE];

    /* headers */
    int headers_count;
    char *headers[];
};

int validate_input(int argc, const char **argv);
int parse_url(const char *url, struct UrlInfo *ui);
void construct_request_message(char *req_msg, const char *uri);
int parse_response_message(struct HttpResponse *hr, char *res);

/* Not test beacuse it needs the server running */
int connect_http_server(const char *host);

#endif
