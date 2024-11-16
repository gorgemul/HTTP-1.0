#include "../lib/unity/unity.h"
#include "../include/client.h"

#define SUCCESS 0
#define ERROR   -1

void setUp(void)
{
}

void tearDown(void)
{
}

void g1_sucess_valid_option_y(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "y",
        };
        int is_saved = -1;

        is_saved = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(1, is_saved);
}

void g1_sucess_valid_option_1(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "1",
        };
        int is_saved = -1;

        is_saved = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(1, is_saved);
}

void g1_sucess_valid_option_0(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "0",
        };
        int is_saved = -1;

        is_saved = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(0, is_saved);
}

void g1_success_valid_option_n(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "n",
        };
        int is_saved = -1;

        is_saved = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(0, is_saved);
}

void g1_error_no_option_provided(void)
{
        int argc = 2;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void g1_error_trailing_whitespace_in_option(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "y ",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g1_error_no_such_option(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "yes",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void test_validate_input_g1(void)
{
        g1_sucess_valid_option_y();
        g1_sucess_valid_option_1();
        g1_sucess_valid_option_0();
        g1_success_valid_option_n();

        g1_error_no_option_provided();
        g1_error_trailing_whitespace_in_option();
        g1_error_no_such_option();
}

void g2_success_string_ip(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://localhost/foo/bar";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("localhost", ui.host);
        TEST_ASSERT_EQUAL_STRING("foo/bar", ui.path);
}

void g2_success_numeric_ip(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://127.0.0.1/something.txt";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("127.0.0.1", ui.host);
        TEST_ASSERT_EQUAL_STRING("something.txt", ui.path);
}

void g2_error_no_host(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http:///something.txt";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g2_error_no_path(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://127.0.0.1/";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void test_parse_url_g2(void)
{
        g2_success_string_ip();
        g2_success_numeric_ip();

        g2_error_no_host();
        g2_error_no_path();
}

void g3_sucess_1(void)
{
        const char *uri = "foo/bar/test.txt";
        char req_msg[REQUEST_MESSAGE_MAX_SIZE] = {0};
        const char *expected =
                "GET /foo/bar/test.txt HTTP/1.0\n" // Initial line
                "User-Agent: MyHttpClient/1.0\n"   // Fixed headers for this toy program
                "\r\n";                            // Blank line(CRLF)

        construct_request_message(req_msg, uri);
        TEST_ASSERT_EQUAL_STRING(expected, req_msg);
}

void test_construct_request_message_g3(void)
{
        g3_sucess_1();
}


void g4_success_initial_line_one_word_message(void)
{
        char response[] =
                "HTTP/1.0 200 OK\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("HTTP/1.0", hr.http_version);
        TEST_ASSERT_EQUAL_STRING("200", hr.status_code);
        TEST_ASSERT_EQUAL_STRING("OK", hr.status_message);
}

void g4_success_initial_line_multiple_word_message(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("HTTP/1.0", hr.http_version);
        TEST_ASSERT_EQUAL_STRING("404", hr.status_code);
        TEST_ASSERT_EQUAL_STRING("Not Found", hr.status_message);
}

void g4_success_headers_one_header(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_INT(1, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);
        free_http_response_struct(&hr);
}

void g4_success_headers_multiple_headers(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "Content-Type: text/plain\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_INT(2, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);
        TEST_ASSERT_EQUAL_STRING("Content-Type: text/plain", hr.headers[1]);
        free_http_response_struct(&hr);
}

void g4_success_headers_different_cases(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "dAtE: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "cOnTENT-tyPe: text/plain\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_INT(2, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);
        TEST_ASSERT_EQUAL_STRING("Content-Type: text/plain", hr.headers[1]);
        free_http_response_struct(&hr);
}

void g4_success_headers_no_whitespace_after_colon_sign(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date:Fri, 31 Dec 1999 23:59:59 GMT\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_INT(1, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);

        free_http_response_struct(&hr);
}

void g4_success_headers_multiple_whitespaces_after_colon_sign(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date:\t Fri, 31 Dec 1999 23:59:59 GMT\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_INT(1, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);
        free_http_response_struct(&hr);
}

void g4_success_content_no_content(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("", hr.content);
}

void g4_success_content_one_line_content(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "\r\n"
                "abcdefghijklmnopqrstuvwxyz1234567890abcdef";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("abcdefghijklmnopqrstuvwxyz1234567890abcdef", hr.content);
}

void g4_success_content_multiple_lines_content(void)
{
        char response[] =
                "HTTP/1.0 200 OK\n"
                "\r\n"
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<body>\n"
                "\n"
                "<h1>My First Heading</h1>\n"
                "<p>My first paragraph.</p>\n"
                "\n"
                "</body>\n"
                "</html>";

        struct HttpResponse hr = {0};

        const char *expected_content =
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<body>\n"
                "\n"
                "<h1>My First Heading</h1>\n"
                "<p>My first paragraph.</p>\n"
                "\n"
                "</body>\n"
                "</html>";

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING(expected_content, hr.content);
}

void g4_success_whole_response(void)
{
        char response[] =
                "HTTP/1.0 200 OK\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "Content-Type: text/html\n"
                "Content-Length: 100\n"
                "\r\n"
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<body>\n"
                "\n"
                "<h1>My First Heading</h1>\n"
                "<p>My first paragraph.</p>\n"
                "\n"
                "</body>\n"
                "</html>";

        struct HttpResponse hr = {0};

        const char *expected_content =
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<body>\n"
                "\n"
                "<h1>My First Heading</h1>\n"
                "<p>My first paragraph.</p>\n"
                "\n"
                "</body>\n"
                "</html>";

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("HTTP/1.0", hr.http_version);
        TEST_ASSERT_EQUAL_STRING("200", hr.status_code);
        TEST_ASSERT_EQUAL_STRING("OK", hr.status_message);
        TEST_ASSERT_EQUAL_INT(3, hr.headers_count);
        TEST_ASSERT_EQUAL_STRING("Date: Fri, 31 Dec 1999 23:59:59 GMT", hr.headers[0]);
        TEST_ASSERT_EQUAL_STRING("Content-Type: text/html", hr.headers[1]);
        TEST_ASSERT_EQUAL_STRING("Content-Length: 100", hr.headers[2]);
        TEST_ASSERT_EQUAL_STRING(expected_content, hr.content);

        free_http_response_struct(&hr);
}

void g4_error_initial_line_no_http_version(void)
{
        char response[] =
                "200 OK\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_initial_line_no_status_code(void)
{
        char response[] =
                "HTTP/1.0 OK\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void g4_error_initial_line_no_status_message(void)
{
        char response[] =
                "HTTP/1.0 200\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_initial_line_code_msg_dont_match(void)
{
        char response[] =
                "HTTP/1.0 200 Created\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_headers_missing_key(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                ":Fri, 31 Dec 1999 23:59:59 GMT\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_headers_missing_value(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date:\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_headers_no_such_header(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Host: example.org\n" // HTTP/1.1 header
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);

}

void g4_error_headers_preceiding_whitespace(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "  Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void g4_error_headers_trailing_whitespace(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT     \n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void g4_error_blank_linke_no_blank_line(void)
{
        char response[] =
                "HTTP/1.0 200 OK\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "Content-Type: text/plain\n"
                "Content-Length: 42\n"
                "abcdefghijklmnopqrstuvwxyz1234567890abcdef";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void test_parse_response_message_g4(void)
{
        g4_success_initial_line_one_word_message();
        g4_success_initial_line_multiple_word_message();
        g4_success_headers_one_header();
        g4_success_headers_multiple_headers();
        g4_success_headers_different_cases();
        g4_success_headers_no_whitespace_after_colon_sign();
        g4_success_headers_multiple_whitespaces_after_colon_sign();
        g4_success_content_no_content();
        g4_success_content_one_line_content();
        g4_success_content_multiple_lines_content();

        g4_success_whole_response();

        g4_error_initial_line_no_http_version();
        g4_error_initial_line_no_status_code();
        g4_error_initial_line_no_status_message();
        g4_error_initial_line_code_msg_dont_match();
        g4_error_headers_missing_key();
        g4_error_headers_missing_value();
        g4_error_headers_no_such_header();
        g4_error_headers_preceiding_whitespace();
        g4_error_headers_trailing_whitespace();
        g4_error_blank_linke_no_blank_line();
}


int main(void)
{
        UNITY_BEGIN();

        RUN_TEST(test_validate_input_g1);
        RUN_TEST(test_parse_url_g2);
        RUN_TEST(test_construct_request_message_g3);
        RUN_TEST(test_parse_response_message_g4);

        UNITY_END();

        return 0;
}
