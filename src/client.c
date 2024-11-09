#include "../include/client.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int validate_input(int argc, const char **argv)
{
	if ((argc != 3) || (strlen(argv[2]) != 1)) { return -1; }

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
        if (sscanf(url, "http://%31[^/]/%63[^\n]", ui->host, ui->path) == 2) {
                return (strlen(ui->host) && strlen(ui->path)) ? 0 : -1;
        }

        return -1;
}

int connect_http_server(const char *host)
{
    int socket_fd = -1;
    struct addrinfo hints, *res, *cur;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, "http", &hints, &res) != 0) { return -1; }

    for (cur = res; cur != NULL; res = res->ai_next) {
        if ((socket_fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(socket_fd, cur->ai_addr, cur->ai_addrlen) == -1) {
            perror("connect");
            close(socket_fd);
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    return (cur == NULL) ? -1 : socket_fd;
}

void construct_request_message(char *req_msg, const char *uri)
{
    /* Initial line */
    strcat(req_msg, "GET");
    strcat(req_msg, " ");
    strcat(req_msg, "/");
    strcat(req_msg, uri);
    strcat(req_msg, " ");
    strcat(req_msg, "HTTP/1.0");
    strcat(req_msg, "\n");

    /* Headers */
    strcat(req_msg, "User-Agent: MyHttpClient/1.0");
    strcat(req_msg, "\n");

    /* Blank line(CRLF) */
    strcat(req_msg, "\r\n");
}
