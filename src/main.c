#include "../include/client.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *get_file_name(char path[])
{
        char *from = path;
        char *to = strchr(path, '/');

        while (to != NULL) {
                from = to + 1;

                to = strchr(from, '/');
        }

        return from;
}

void save_file(char name[], char content[])
{
        FILE *file = fopen(name, "w");
        fputs(content, file);
        fclose(file);
}

int main(int argc, const char **argv)
{
        int is_saved = 0;
        int socket_fd = -1;
        int recv_bytes = 0;
        int success = 0;
        char req_buf[REQUEST_MESSAGE_MAX_SIZE] = {0};
        char res_buf[RECV_MESSAGE_MAX_SIZE] = {0};
        struct UrlInfo ui = {0};
        struct HttpResponse hr = {0};

        if ((is_saved = validate_input(argc, argv)) == -1) {
                fprintf(stderr, "USAGE: <program> <url> <'y'|'1'|'n'|'0'>");
                goto ret;
        }

        if (parse_url(argv[1], &ui) == -1) {
                fprintf(stderr, "ERROR: Invalid url");
                goto ret;
        }

        if ((socket_fd = connect_http_server(ui.host)) == -1) {
                fprintf(stderr, "ERROR: connect_http_server");
                goto ret;
        }

        printf("Sucessfully connect to server: %s:80\n", ui.host);

        construct_request_message(req_buf, ui.path);

        printf("Requesting resouces on /%s...\n", ui.path);

        if (send(socket_fd, req_buf, strlen(req_buf), 0) == -1) {
                fprintf(stderr, "ERROR: send");
                goto clean;
        }

        printf("Waiting for server response...\n");

        switch (recv_bytes = recv(socket_fd, res_buf, RECV_MESSAGE_MAX_SIZE-1, 0)) {
        case -1:
                fprintf(stderr, "ERROR: recv");
                goto clean;
        case 0:
                fprintf(stderr, "ERROR: Server close connection before responsing");
                goto clean;
        default:
                if (parse_response_message(&hr, res_buf) == -1) {
                        fprintf(stderr, "ERROR: parse_response_message");
                        goto clean;
                }

                success = 1;

                if (strcmp(hr.status_code, "200") != 0) {
                        printf("REQUEST FAIL: %s %s\n", hr.status_code, hr.status_message);
                        goto clean;
                }

                if (is_saved) {
                        char *file_name = get_file_name(ui.path);
                        save_file(file_name, hr.content);
                        printf("successfully saved file: %s\n", file_name);
                } else {
                        printf("Receving content====>\n");
                        printf("%s\n", hr.content);
                }
        }


clean:
        free_http_response_struct(&hr);
        close(socket_fd);
        printf("Cleaning allocating memory...\n");
        printf("Closing connection...\n");
ret:
        return success ? 0 : 1;
}
