/****************************************************************************
 * dictionary.h
 *
 * Application Security, Assignment 1
 *
 * Adapted from code written by Ben Halperin
 ***************************************************************************/

#ifndef DICTIONARY_H
#define DICTIONARY_H

#ifndef _WIN32
#include <stdbool.h>
#endif
#include <stdio.h>

// maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

#define HASH_SIZE 2000

#define MAX_MISSPELLED 1000

typedef struct node
{
    char word[LENGTH + 1];
    struct node* next;
}
node;

typedef node* hashmap_t;


/**
 * Already implemented in dictionary.c
 **/
int hash_function(const char* word);

#endif // DICTIONARY_H
