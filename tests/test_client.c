#include "../lib/unity/unity.h"
#include "../include/client.h"

#define SUCCESS 0
#define FAIL   -1

void setUp(void)
{
}

void tearDown(void)
{
}

void test_validate_input_correct_1(void)
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

void test_validate_input_correct_2(void)
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

void test_validate_input_correct_3(void)
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

void test_validate_input_correct_4(void)
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

void test_validate_input_incorrect_1(void)
{
        int argc = 2;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(FAIL, retval);
}


void test_validate_input_incorrect_2(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "y ",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(FAIL, retval);
}

void test_validate_input_incorrect_3(void)
{
        int argc = 3;
        const char *argv[] = {
                "./httpclient",
                "http://127.0.0.1/foo/bar/baz.txt",
                "yes",
        };
        int retval = -1;

        retval = validate_input(argc, argv);

        TEST_ASSERT_EQUAL_INT(FAIL, retval);
}

void test_parse_url_correct_1(void)
{
        struct UrlInfo ui;
        const char *url = "http://localhost/foo/bar";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("localhost", ui.host);
        TEST_ASSERT_EQUAL_STRING("foo/bar", ui.path);
}

void test_parse_url_correct_2(void)
{
        struct UrlInfo ui;
        const char *url = "http://127.0.0.1/something.txt";
        int retval = -1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(SUCCESS, retval);

        TEST_ASSERT_EQUAL_STRING("127.0.0.1", ui.host);
        TEST_ASSERT_EQUAL_STRING("something.txt", ui.path);
}

/* NO HOST */
void test_parse_url_incorrect_1(void)
{
        struct UrlInfo ui;
        const char *url = "http:///something.txt";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(FAIL, retval);
}

/* NO PATH */
void test_parse_url_incorrect_2(void)
{
        struct UrlInfo ui;
        const char *url = "http://127.0.0.1/";
        int retval = 1;

        retval = parse_url(url, &ui);

        TEST_ASSERT_EQUAL_INT(FAIL, retval);
}


int main(void)
{
        UNITY_BEGIN();

        RUN_TEST(test_validate_input_correct_1);
        RUN_TEST(test_validate_input_correct_2);
        RUN_TEST(test_validate_input_correct_3);
        RUN_TEST(test_validate_input_correct_4);

        RUN_TEST(test_validate_input_incorrect_1);
        RUN_TEST(test_validate_input_incorrect_2);
        RUN_TEST(test_validate_input_incorrect_3);

        RUN_TEST(test_parse_url_correct_1);
        RUN_TEST(test_parse_url_correct_2);

        RUN_TEST(test_parse_url_incorrect_1);
        RUN_TEST(test_parse_url_incorrect_2);

        UNITY_END();

        return 0;
}
