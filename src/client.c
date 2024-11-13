#include "../include/client.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

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

int get_index(const char *code)
{
        const char *codes[TOTAL_STATUS] = {
                "200",
                "201",
                "202",
                "204",

                "301",
                "302",
                "304",

                "400",
                "401",
                "403",
                "404",

                "500",
                "501",
                "502",
                "503",
        };

        for (int i = 0; i < TOTAL_STATUS; i++) {
                if (strcmp(codes[i], code) == 0) { return i; }
        }

        return -1;
}

/* All valid status code in HTTP/1.0
    2xx:
    200 - OK
    201 - Created
    202 - Aceepted
    204 - No Content

    3xx:
    301 - Moved Permanently
    302 - Moved Temporarily
    304 - Not Modified

    4xx:
    400 - Bad Request
    401 - Unauthorized
    403 - Forbidden
    404 - Not Found

    5xx:
    500 - Internal Server Error
    501 - Not Implemented
    502 - Bad Gateway
    503 - Service Unavailable
*/
int is_valid_status_code(const char *code)
{
        if (strlen(code) != 3) { return 0; }

        const char *codes[TOTAL_STATUS] = {
                "200",
                "201",
                "202",
                "204",

                "301",
                "302",
                "304",

                "400",
                "401",
                "403",
                "404",

                "500",
                "501",
                "502",
                "503",
        };

        for (int i = 0; i < TOTAL_STATUS; i++) {
                if (strcmp(codes[i], code) == 0) { return 1; }
        }

        return 0;
}

int is_code_msg_match(const char* code, const char *msg)
{
        int match_index = -1;

        const char *msgs[TOTAL_STATUS] = {
                "OK",
                "Created",
                "Aceepted",
                "No Content",

                "Moved Permanently",
                "Moved Temporarily",
                "Not Modified",

                "Bad Request",
                "Unauthorized",
                "Forbidden",
                "Not Found",

                "Internal Server Error",
                "Not Implemented",
                "Bad Gateway",
                "Service Unavailable",
        };
        if ((match_index = get_index(code)) == -1) { return 0; }

        return (strcmp(msgs[match_index], msg) == 0) ? 1 : 0;
}

int scan_initial_line(struct HttpResponse *hr, char *init_line)
{
        int counter = 1;
        int token_length = 0;
        char version_buf[VERSION_MAX_LENGTH] = {0};
        char code_buf[CODE_MAX_LENGTH] = {0};
        char msg_buf[MESSAGE_MAX_LENGTH] = {0};
        char *from = init_line;
        char *to = NULL;

        while (counter <= 3) {
                switch (counter) {
                case 1:
                        if ((to = strchr(init_line, ' ')) == NULL) { return -1; }
                        token_length = to - from;

                        if (token_length >= VERSION_MAX_LENGTH) { return -1; }
                        memcpy(version_buf, from, token_length);
                        version_buf[token_length] = '\0';
                        if (strcmp(version_buf, "HTTP/1.0") != 0) { return -1; }
                        from = to + 1;
                        strcpy(hr->http_version, version_buf);
                        counter++;
                        break;
                case 2:
                        if ((to = strchr(from, ' ')) == NULL) { return -1; }
                        token_length = to - from;
                        if (token_length >= CODE_MAX_LENGTH) { return -1; }
                        memcpy(code_buf, from, token_length);
                        code_buf[token_length] = '\0';
                        if (!is_valid_status_code(code_buf)) { return -1; }
                        strcpy(hr->status_code, code_buf);
                        from = to + 1;
                        counter++;
                        break;
                default:
                        token_length = strlen(from);
                        if (token_length == 0 || token_length >= MESSAGE_MAX_LENGTH) { return -1; }
                        memcpy(msg_buf, from, token_length);
                        msg_buf[token_length] = '\0';
                        if (!is_code_msg_match(hr->status_code, msg_buf)) { return -1; }
                        strcpy(hr->status_message, msg_buf);
                        counter++;
                        break;
                }
        }

        return 0;
}

/* >=0 represents advance distance, -1 represents last line */
int scan_content_line(char content[], char *line)
{
        char *from = line;
        char *to = strchr(line, '\n');
        int is_empty_line = (from == to);
        int is_last_line = (to == NULL);

        if (is_empty_line) {
                strcat(content, "\n");
                return 1;
        }

        if (is_last_line) {
                strcat(content, line);
                return -1;
        }

        int length = (to - from) + 1;
        char *buf = malloc(sizeof(*buf) * length+1);

        memcpy(buf, line, length);
        buf[length] = '\0';

        strcat(content, buf);

        free(buf);

        return length;
}

void scan_content(char *buf, char *content)
{
        int advance_distance = 0;
        char *line_index = content;

        do {
                advance_distance = scan_content_line(buf, line_index);
                line_index += advance_distance;
        } while (advance_distance != -1);
}

/* Helper functions */

int parse_response_message(struct HttpResponse *hr, char *res)
{
        int is_first_line = 1;
        int has_ignore_blank_line = 0;

        char *token = strtok(res, "\n");

        do {
                if (is_first_line) {
                        if (scan_initial_line(hr, res) == -1) { return -1; }
                        is_first_line = 0;
                        token = strtok(NULL, "\n");
                        continue;
                }

                if (token[0] == '\r') {
                        has_ignore_blank_line = 1;
                        token = strtok(NULL, "\0");
                        continue;
                }

                if (!has_ignore_blank_line) {
                        token = strtok(NULL, "\n");
                        continue;
                }

                scan_content(hr->content, token);
                token = strtok(NULL, "\0");

        } while (token != NULL);

        return 0;
}
