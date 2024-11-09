#include "../include/client.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char **argv)
{
        int is_saved = 0;
        int socket_fd = -1;
        int recv_bytes = 0;
        char req_buf[REQUEST_MESSAGE_MAX_SIZE] = {0};
        char res_buf[RECV_MESSAGE_MAX_SIZE] = {0};
        struct UrlInfo ui = {0};

        if ((is_saved = validate_input(argc, argv)) == -1) {
                fprintf(stderr, "USAGE: <program> <url> <'y'|'1'|'n'|'0'>");
                exit(1);
        }

        if (parse_url(argv[1], &ui) == -1) {
                fprintf(stderr, "ERROR: Invalid url");
                exit(1);
        }

        if ((socket_fd = connect_http_server(ui.host)) == -1) {
                fprintf(stderr, "ERROR: connect_http_server");
                exit(1);
        }

        printf("Sucessfully connect to server: %s:80\n", ui.host);

        construct_request_message(req_buf, ui.path);

        printf("Requesting resouces on /%s...\n", ui.path);

        if (send(socket_fd, req_buf, strlen(req_buf), 0) == -1) {
                close(socket_fd);
                fprintf(stderr, "ERROR: send");
                exit(1);
        }

        printf("Waiting for server response...\n");

        switch (recv_bytes = recv(socket_fd, res_buf, RECV_MESSAGE_MAX_SIZE-1, 0)) {
                case -1:
                        close(socket_fd);
                        fprintf(stderr, "ERROR: recv");
                        exit(1);
                case 0:
                        close(socket_fd);
                        fprintf(stderr, "ERROR: Server close connection before responsing");
                        exit(1);
                default:
                        return 1; // TODO: Parse response buffer
        }

        return 0;
}
