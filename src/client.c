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

/* Helper functions */
int is_valid_status_code(const char str_code[])
{
        int code = 0;

        if ((code = atoi(str_code)) == 0) { return 0; }

        switch (code) {
        case OK:
        case CREATED:
        case ACCEPTED:
        case NO_CONTENT:
        case MOVED_PERMANENTLY:
        case MOVED_TEMPORARILY:
        case NOT_MODIFIED:
        case BAD_REQUEST:
        case UNAUTHORIZED:
        case FORBIDDEN:
        case NOT_FOUND:
        case INTERNAL_SERVER_ERROR:
        case NOT_IMPLEMENTED:
        case BAD_GATEWAY:
        case SERVICE_UNAVAILABLE:
                return 1;
        default:
                return 0;
        }
}

const char *code_to_msg(int code)
{
        switch (code) {
        case OK:
                return "OK";
        case CREATED:
                return "Created";
        case ACCEPTED:
                return "Accpeted";
        case NO_CONTENT:
                return "No Content";
        case MOVED_PERMANENTLY:
                return "Moved Permanently";
        case MOVED_TEMPORARILY:
                return "Moved Temporarily";
        case NOT_MODIFIED:
                return "Not Modified";
        case BAD_REQUEST:
                return "Bad Request";
        case UNAUTHORIZED:
                return "Unauthorized";
        case FORBIDDEN:
                return "Forbidden";
        case NOT_FOUND:
                return "Not Found";
        case INTERNAL_SERVER_ERROR:
                return "Internal Server Error";
        case NOT_IMPLEMENTED:
                return "Not Implemented";
        case BAD_GATEWAY:
                return "Bad Gateway";
        case SERVICE_UNAVAILABLE:
                return "Service Unavailable";
        default:
                return "ERROR";
        }
}

int is_code_msg_match(const char* str_code, const char *got_msg)
{
        int code = atoi(str_code); // Since check before, doesn't do validation here

        return (strcmp(code_to_msg(code), got_msg) == 0) ? 1 : 0;
}

int scan_http_version(struct HttpResponse *hr, char *from, char *to)
{
        char buf[VERSION_MAX_LENGTH] = {0};
        int str_len = to - from;

        if (str_len >= VERSION_MAX_LENGTH) { return -1; }

        memcpy(buf, from, str_len);
        buf[str_len] = '\0';

        if (strcmp(buf, "HTTP/1.0") != 0) { return -1; }

        strcpy(hr->http_version, buf);

        return 0;
}

int scan_status_code(struct HttpResponse *hr, char *from, char *to)
{
        char buf[CODE_MAX_LENGTH] = {0};
        int str_len = to - from;

        if (str_len >= CODE_MAX_LENGTH) { return -1; }

        memcpy(buf, from, str_len);
        buf[str_len] = '\0';

        if (!is_valid_status_code(buf)) { return -1; }

        strcpy(hr->status_code, buf);

        return 0;
}

int scan_status_message(struct HttpResponse *hr, char *from)
{
        char buf[MESSAGE_MAX_LENGTH] = {0};
        int str_len = strlen(from);

        if ((str_len == 0) || (str_len >= MESSAGE_MAX_LENGTH)) { return -1; }

        memcpy(buf, from, str_len);
        buf[str_len] = '\0';

        if (!is_code_msg_match(hr->status_code, buf)) { return -1; }

        strcpy(hr->status_message, buf);

        return 0;
}

int scan_initial_line(struct HttpResponse *hr, char *initial_line)
{
        int counter = 1;
        char *from = initial_line;
        char *to = NULL;

        while (counter <= 3) {
                switch (counter) {
                case 1:
                        if ((to = strchr(initial_line, ' ')) == NULL) { return -1; }
                        if (scan_http_version(hr, from, to) == -1) { return -1; }
                        from = to + 1;
                        counter++;
                        break;
                case 2:
                        if ((to = strchr(from, ' ')) == NULL) { return -1; }
                        if (scan_status_code(hr, from, to) == -1) { return -1; }
                        from = to + 1;
                        counter++;
                        break;
                default:
                        if (scan_status_message(hr, from) == -1) { return -1; }
                        counter++;
                        break;
                }
        }

        return 0;
}

/* >=0 represents advance distance, -1 represents last line */
int scan_one_line_content(char content[], char *line)
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
        char *buf = malloc(sizeof(*buf) * (length+1));

        memcpy(buf, line, length);
        buf[length] = '\0';

        strcat(content, buf);

        free(buf);

        return length;
}

void scan_multiple_lines_content(char *buf, char *content)
{
        int advance_distance = 0;
        char *line_index = content;

        do {
                advance_distance = scan_one_line_content(buf, line_index);
                line_index += advance_distance;
        } while (advance_distance != -1);
}
/* Helper functions */

void free_http_response_struct(struct HttpResponse *hr)
{
    if (hr->headers_count == 0) { return; }

    for (int i = 0; i < hr->headers_count; i++) {
        free(hr->headers[i]);
    }

    free(hr->headers);
}

// TODO: Should hanlde the memory allocation error
int scan_one_line_header(struct HttpResponse *hr, char line[])
{
        char *from = line;
        char *to = strchr(line, ':');

        int is_no_colon = (to == NULL);
        int is_no_key = (to == from);
        int is_no_val = (*(to + 1) == '\0');

        if (is_no_colon || is_no_key || is_no_val) { return -1; }

        int key_len = to - from;
        char *key_buf = malloc(sizeof(char) * (key_len+1));
        memcpy(key_buf, from, key_len);
        key_buf[key_len] = '\0';

        from = to + 1;

        int val_len = strlen(from);
        char *val_buf = malloc(sizeof(char) * (val_len+1));
        strcpy(val_buf, from);

        int header_len = val_len + key_len + 1;

        if (header_len >= HEADER_MAX_SIZE) {
                free(key_buf);
                free(val_buf);
                return -1;
        }

        hr->headers_count++;

        if (hr->headers_count == 1) {
                hr->headers = malloc(sizeof(char*) * hr->headers_count);
        } else {
                hr->headers = realloc(hr->headers, sizeof(char*) * hr->headers_count);
        }
        hr->headers[(hr->headers_count)-1] = malloc(sizeof(char) * header_len);
        strcat(hr->headers[(hr->headers_count)-1], key_buf);
        strcat(hr->headers[(hr->headers_count)-1], ":");
        strcat(hr->headers[(hr->headers_count)-1], val_buf);

        free(key_buf);
        free(val_buf);

        return 0;
}

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

                // TODO: Need to handle free
                if (token[0] == '\r') {
                        has_ignore_blank_line = 1;
                        token = strtok(NULL, "\0");
                        continue;
                }

                if (!has_ignore_blank_line) {
                        if (scan_one_line_header(hr, token) == -1) {
                                free_http_response_struct(hr);
                                return -1;
                        }
                        token = strtok(NULL, "\n");
                        continue;
                }

                scan_multiple_lines_content(hr->content, token);
                token = strtok(NULL, "\0");

        } while (token != NULL);

        return 0;
}
