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

void test_validate_input_success_1(void)
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

void test_validate_input_sucess_2(void)
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

void test_validate_input_sucess_3(void)
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

void test_validate_input_sucess_4(void)
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

void test_validate_input_error_1(void)
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


void test_validate_input_error_2(void)
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

void test_validate_input_error_3(void)
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

void test_parse_url_sucess_1(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://localhost/foo/bar";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("localhost", ui.host);
        TEST_ASSERT_EQUAL_STRING("foo/bar", ui.path);
}

void test_parse_url_sucess_2(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://127.0.0.1/something.txt";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("127.0.0.1", ui.host);
        TEST_ASSERT_EQUAL_STRING("something.txt", ui.path);
}

/* NO HOST */
void test_parse_url_error_1(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http:///something.txt";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

/* NO PATH */
void test_parse_url_error_2(void)
{
        struct UrlInfo ui = {0};
        const char *url = "http://127.0.0.1/";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void test_construct_request_message(void)
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

void test_parse_response_message_sucess_init_line_1(void)
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

void test_parse_response_message_sucess_init_line_2(void)
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

void test_parse_response_message_sucess_content_1(void)
{
        char response[] =
                "HTTP/1.0 404 Not Found\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);
        TEST_ASSERT_EQUAL_STRING("", hr.content);
}

void test_parse_response_message_sucess_content_2(void)
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

void test_parse_response_message_sucess_content_3(void)
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

void test_parse_response_message_error_init_line_1(void)
{
        char response[] =
                "200 OK\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void test_parse_response_message_error_init_line_2(void)
{
        char response[] =
                "HTTP/1.0 OK\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}


void test_parse_response_message_error_init_line_3(void)
{
        char response[] =
                "HTTP/1.0 200\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void test_parse_response_message_error_init_line_4(void)
{
        char response[] =
                "HTTP/1.0 200 Created\n"
                "\r\n";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

void test_parse_response_message_error_blank_line(void)
{
        char response[] =
                "HTTP/1.0 200 Created\n"
                "Date: Fri, 31 Dec 1999 23:59:59 GMT\n"
                "Content-Type: text/plain\n"
                "Content-Length: 42\n"
                "abcdefghijklmnopqrstuvwxyz1234567890abcdef";

        struct HttpResponse hr = {0};

        int retval = parse_response_message(&hr, response);

        TEST_ASSERT_EQUAL_INT(ERROR, retval);
}

int main(void)
{
        UNITY_BEGIN();

        RUN_TEST(test_validate_input_success_1);
        RUN_TEST(test_validate_input_sucess_2);
        RUN_TEST(test_validate_input_sucess_3);
        RUN_TEST(test_validate_input_sucess_4);

        RUN_TEST(test_validate_input_error_1);
        RUN_TEST(test_validate_input_error_2);
        RUN_TEST(test_validate_input_error_3);

        RUN_TEST(test_parse_url_sucess_1);
        RUN_TEST(test_parse_url_sucess_2);

        RUN_TEST(test_parse_url_error_1);
        RUN_TEST(test_parse_url_error_2);

        RUN_TEST(test_construct_request_message);

        RUN_TEST(test_parse_response_message_sucess_init_line_1);
        RUN_TEST(test_parse_response_message_sucess_init_line_2);

        RUN_TEST(test_parse_response_message_sucess_content_1);
        RUN_TEST(test_parse_response_message_sucess_content_2);
        RUN_TEST(test_parse_response_message_sucess_content_3);

        RUN_TEST(test_parse_response_message_error_init_line_1);
        RUN_TEST(test_parse_response_message_error_init_line_2);
        RUN_TEST(test_parse_response_message_error_init_line_3);
        RUN_TEST(test_parse_response_message_error_init_line_4);

        RUN_TEST(test_parse_response_message_error_blank_line);

        UNITY_END();

        return 0;
}
