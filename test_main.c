#include <check.h>
#include "dictionary.h"
#include <stdlib.h>

#define DICTIONARY "wordlist.txt"
#define TESTDICT "test_worlist.txt"

START_TEST(test_dictionary_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    ck_assert(load_dictionary(TESTDICT, hashtable));
    // Here we can test if certain words ended up in certain buckets
    // to ensure that our load_dictionary works as intended. I leave
    // this as an exercise.
}
END_TEST
    
START_TEST(test_check_word_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* testwords[7];
    testwords[0] = "test";
    testwords[1] = "Test";
    testwords[2] = "tesT";
    testwords[3] = "tESt";
    testwords[4] = "te.st";
    testwords[5] = "ith";
    testwords[6] = ",test?";

    ck_assert(check_word(testwords[0], hashtable));
    ck_assert(check_word(testwords[1], hashtable));
    ck_assert(check_word(testwords[2], hashtable));
    ck_assert(check_word(testwords[3], hashtable));
    ck_assert(!check_word(testwords[4], hashtable));
    ck_assert(!check_word(testwords[5], hashtable));
    ck_assert(check_word(testwords[6], hashtable));
}
END_TEST
START_TEST(test_check_word_abnormal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* testwords[6];
    testwords[0] = "!@#$%^&*()test!@#$%^&*()";
    testwords[1] = "\1\1\1\1test\1\1\1\1\1";
    testwords[2] = "\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0\xA0";
    testwords[3] = "w\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0\xA0w";

    ck_assert(check_word(testwords[0], hashtable));
    ck_assert(!check_word(testwords[1], hashtable));
    ck_assert(check_word(testwords[2], hashtable));
    ck_assert(!check_word(testwords[3], hashtable));
}
END_TEST

START_TEST(test_check_words_wordlist_01)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[3];
    expected[0] = "sogn";
    expected[1] = "skyn";
    expected[2] = "betta";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test1.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 3);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[1]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[2]) == 0);
}
END_TEST

START_TEST(test_check_words_wordlist_02)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test2.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST
    
START_TEST(test_check_words_wordlist_03)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test3.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST
    
START_TEST(test_check_words_wordlist_04)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[1];
    expected[0] = "ddd";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test4.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 1);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
}
END_TEST

START_TEST(test_check_words_wordlist_05)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test5.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 0);
}
END_TEST
    
START_TEST(test_check_words_wordlist_06)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[5];
    expected[0] = "w\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0w";
    expected[1] = "te.st";
    expected[2] = "ith";
    expected[3] = "\1\1\1\1test\1\1\1\1";
    expected[4] = "w\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0w";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test6.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 5);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[1]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[2]) == 0);
    ck_assert_msg(strcmp(misspelled[3], expected[3]) == 0);
    ck_assert_msg(strcmp(misspelled[4], expected[4]) == 0);
}
END_TEST
    
START_TEST(test_check_words_wordlist_07)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[1];
    expected[0] = "aaaa";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test7.txt", "rb");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 5);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[3], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[4], expected[0]) == 0);
}
END_TEST

Suite *
check_word_suite(void)
{
    Suite * suite;
    TCase * check_word_case;
    suite = suite_create("check_word");
    check_word_case = tcase_create("Core");
    tcase_add_test(check_word_case, test_check_word_normal);
    tcase_add_test(check_word_case, test_check_word_abnormal);
    tcase_add_test(check_word_case, test_check_words_wordlist_01);
    tcase_add_test(check_word_case, test_check_words_wordlist_02);
    tcase_add_test(check_word_case, test_check_words_wordlist_03);
    tcase_add_test(check_word_case, test_check_words_wordlist_04);
    tcase_add_test(check_word_case, test_check_words_wordlist_05);
    tcase_add_test(check_word_case, test_check_words_wordlist_06);
    tcase_add_test(check_word_case, test_check_words_wordlist_07);
    suite_add_tcase(suite, check_word_case);

    return suite;
}

int
main(void)
{
    int failed;
    Suite *suite;
    SRunner *runner;
    
    suite = check_word_suite();
    runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

