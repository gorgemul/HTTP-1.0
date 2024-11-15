#ifndef CLIENT_H
#define CLIENT_H

#define REQUEST_MESSAGE_MAX_SIZE 2048
#define RECV_MESSAGE_MAX_SIZE 204800
#define CONTENT_MAX_SIZE 8192
#define HEADER_MAX_SIZE 8192
#define TOTAL_STATUS 15
#define VERSION_MAX_LENGTH 9
#define CODE_MAX_LENGTH 4
#define MESSAGE_MAX_LENGTH 30

/* All valid status code in HTTP/1.0 */
enum EResponseStatus {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,

    MOVED_PERMANENTLY = 301,
    MOVED_TEMPORARILY = 302,
    NOT_MODIFIED = 304,

    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,

    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,

    TOTAL_RSE_STATUS = 15,
};

struct UrlInfo {
	char host[32];
	char path[64];
};

struct HttpResponse {
    /* Initial line */
    char http_version[VERSION_MAX_LENGTH];
    char status_code[CODE_MAX_LENGTH];
    char status_message[MESSAGE_MAX_LENGTH];
    /* content */
    char content[CONTENT_MAX_SIZE];
    /* headers */
    int headers_count;
    char **headers;
};

int validate_input(int argc, const char **argv);
int parse_url(const char *url, struct UrlInfo *ui);
void construct_request_message(char *req_msg, const char *uri);
int parse_response_message(struct HttpResponse *hr, char *res);

/* Not test beacuse it needs the server running */
int connect_http_server(const char *host);
/* Just don't know a good way to test a clean function, not test rn */
void free_http_response_struct(struct HttpResponse *hr);

#endif
