


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"


#define DICTIONARY "wordlist.txt"
#define TESTDICT "test_worlist.txt"

int _test_01();
int _test_02();
int _test_03();
int _test_04();
int _test_05();
int _test_06();
int _test_07();
int _test_08();
int _test_files(char* testfile, char* dictfile);
void log_start(char* msg);
void log_msg(char* msg);
void log_msg_i1(char* msg, int i1);
void log_res_i1(char* msg, int i1);
void log_err(char* msg);

int main(int argc, char** argv)
{
    int ret = 0;
    int ec;
    
    if(argc==1)
    {
        ec = _test_01();
        ec = _test_02();
        ec = _test_03();
        ec = _test_04();
        ec = _test_05();
        ec = _test_06();
        ec = _test_07();
        ec = _test_08();
    }
    else if(argc==3)
    {
        ec = _test_files(argv[1], argv[2]);
    }
    ret = 1;

EXIT_RET:
    return ret;
}

void free_next(hashmap_t hash)
{
    if(hash!=NULL)
    {
        if(hash->next!=NULL)
        {
            free_next(hash->next);
            hash->next = NULL;
        }
    }
}
void clean(hashmap_t hashtable[], char* mspell[])
{
    int i;
    if(hashtable!=NULL)
    {
        for(i=0; i<HASH_SIZE; i++)
        {
            if(hashtable[i]!=NULL)
            {
                free_next(hashtable[i]);
                free(hashtable[i]);
                hashtable[i] = NULL;
            }
        }
    }
    if(mspell!=NULL)
    {
        for(i=0; i<MAX_MISSPELLED; i++)
        {
            if(mspell[i]!=NULL)
            {
                free(mspell[i]);
                mspell[i]=NULL;
            }
        }
    }
}

int _test_01()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 01 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test1'...");
    fd = fopen("test1.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==3)   log_res_i1("PASS - %d==3 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=3 invalid words found", ec);

    fclose(fd);
    
    log_msg("-- Loading test file 'tolower'...");
    fd = fopen("tolower.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==3)   log_res_i1("PASS - %d==3 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=3 invalid words found", ec);

    fclose(fd);

EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}
int _test_02()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    
    log_start("---- Test 02 ----------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("Testing words...");
    if(!check_word("test", hashtable)) log_err("'test' err");
    if(!check_word("Test", hashtable)) log_err("'Test' err");
    if(!check_word("tesT", hashtable)) log_err("'tesT' err");
    if(!check_word("tESt", hashtable)) log_err("'tESt' err");
    if(!check_word("!@#$%^&*()test!@#$%^&*()", hashtable)) log_err("'!@#$%^&*()test!@#$%^&*()' err");
    if(check_word("te.st", hashtable)) log_err("'te.st' err");
    if(check_word("ith", hashtable)) log_err("'ith' err");
    if(check_word("\1\1\1\1test\1\1\1\1\1", hashtable)) log_err("'\\1' err");
    if(!check_word("\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0", hashtable)) log_err("'\\xA0' err");
    if(check_word("w\xA0\xA0\xA0\xA0test\xA0\xA0\xA0\xA0w", hashtable)) log_err("'w\\xA0' err");
    

EXIT_RET:
    clean(hashtable, NULL);
    return ret;
}


int _test_03()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 03 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test2'...");
    fd = fopen("test2.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==0)   log_res_i1("PASS - %d==0 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=0 invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}

int _test_04()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 04 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test4'...");
    fd = fopen("test4.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==1)   log_res_i1("PASS - %d==1 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=1 invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}

int _test_05()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 05 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test5'...");
    fd = fopen("test5.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==0)   log_res_i1("PASS - %d==0 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=0 invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}
int _test_06()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 06 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test3'...");
    fd = fopen("test3.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==0)   log_res_i1("PASS - %d==0 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=0 invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}
int _test_07()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 07 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test6'...");
    fd = fopen("test6.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==5)   log_res_i1("PASS - %d==5 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=5 invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}
int _test_08()
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];
    int i;

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Test 08 ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(DICTIONARY, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file 'test7'...");
    fd = fopen("test7.txt", "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    if(ec==5)   log_res_i1("PASS - %d==5 invalid words found", ec);
    else        log_res_i1("FAIL - %d!=5 invalid words found", ec);
    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}

int _test_files(char* testfile, char* dictfile)
{
    int ret = 0;
    int ec;
    hashmap_t hashtable[HASH_SIZE];
    FILE* fd;
    char* mspell[MAX_MISSPELLED];

    memset(mspell, 0, sizeof(char*)*MAX_MISSPELLED);
    
    log_start("---- Testing cmdline files ------------------");
    log_msg("Loading dictionary...");
    ec = load_dictionary(dictfile, hashtable);
    if(!ec)
    {
        ret = -1;
        log_err("Failed to load dictionary!");
        goto EXIT_RET;
    }

    log_msg("-- Loading test file...");
    fd = fopen(testfile, "rb");
    if(!fd)
    {
        ret = -2;
        log_err("Failed to open test file.");
        goto EXIT_RET;
    }

    ec = check_words(fd, hashtable, mspell);
    log_msg_i1("%d invalid words found", ec);

    fclose(fd);
    
EXIT_RET:
    clean(hashtable, mspell);
    return ret;
}

void log_start(char* msg)
{
    fprintf(stdout, msg); fprintf(stdout, "\n");
}

void log_msg(char* msg)
{
    fprintf(stdout, " > ");
    fprintf(stdout, msg); fprintf(stdout, "\n");
}
void log_msg_i1(char* msg, int i1)
{
    fprintf(stdout, " > ");
    fprintf(stdout, msg, i1); fprintf(stdout, "\n");
}
void log_res_i1(char* msg, int i1)
{
    fprintf(stdout, "@> ");
    fprintf(stdout, msg, i1); fprintf(stdout, "\n");
}

void log_err(char* msg)
{
    fprintf(stdout, "!> ");
    fprintf(stdout, msg); fprintf(stdout, "\n");
}