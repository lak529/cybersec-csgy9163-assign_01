
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"



//max size, including puncs, for a word token to check
#define MAXWORDCHECK (LENGTH*2)
#define MAXLINE 8192


int strip_punks(const char srcword[], char destword[])
{
    int ret = 0;
    int ps = 0;
    int pe = 0;
    char c;
    int i;
    
    const char* PUNCS = "`~!@#$%^&*()_-+=[]\\|}{'\";:,./?><";
    const int PUNCSNUM = 32;

    memset(destword, 0, LENGTH+1);
    pe = strnlen(srcword,MAXWORDCHECK)-1;

    //strip pre punc
    while(ps<=pe)
    {
        c=srcword[ps];
        if(c=='\0') { break; } //break out, we're at the end of the valid string
        for(i=0; i<PUNCSNUM; i++)
        {
            if(c==PUNCS[i]) { ps++; c=0; break; }
        }
        if(c!=0) break; //break out, we have a letter
    }

    //strip post apoc punk
    while(pe>=ps)
    {
        c=srcword[pe];
        if(c=='\0') { pe--; continue; }
        for(i=0; i<PUNCSNUM; i++)
        {
            if(c==PUNCS[i]) { pe--; c=0; break; }
        }
        if(c!=0) break; //break out, we have a letter
    }

    //invalid word
    if(ps>pe){ret=0; goto EXIT_RET;}
    //word too long for output buffer
    if(pe-ps+1 > LENGTH) {ret=-1; goto EXIT_RET; }

    //copy the word over, so we can work on it
    memcpy(destword, srcword+ps, pe-ps+1);
    ret = pe-ps+1;

EXIT_RET:
    return ret;
}

int check_words(FILE* fp, hashmap_t hashtable[], char* misspelled[])
{
    int ret = 0;
    int errcount;
    int linelen;
    char c;
    int ws,we;
    int eol = 0;
    int index = 0;

    char* word;
    char tword[MAXWORDCHECK+1];
    char line[MAXLINE+1];

    errcount = 0;
    while(!feof(fp))
    {
        memset(line,0,MAXLINE+1);
        linelen = 0;
        eol = 0;

        //read the line
        while(linelen<MAXLINE && !(eol || feof(fp)))
        {
            c = fgetc(fp);
            if(c==0 || c==(char)0xFF) break;
            else if(c=='\n' || c=='\r') { eol=1; break; }
            else
            {
                if(c==' ') c = '\0';    //work the tok nulls in
                line[linelen++] = c;
            }
        }
        
        //we have a word, but we're not at the end of a line (and its not eof)
        if(linelen>0 && !eol && !feof(fp))
        {
            c=1;
            //chew up the rest of the line
            while(c!=0 && c!=(int)0xFF && c!='\n' && c!='\r' && !feof(fp))
            {
                c = fgetc(fp);
            }
            continue;//next line!
        }

        //space tokenize
        ws=0;
        while(ws < linelen)
        {
            //find wordstart
            while(ws<linelen && line[ws]=='\0') ws++;
            we=ws;
            //find wordend
            while(we<linelen && line[we]!='\0') we++;

            if(ws>=linelen) break;  //no more words on this line
            if(we-ws+1 > LENGTH) { ws=we; continue; }   //word too long
            memset(tword, 0, MAXWORDCHECK+1);
            memcpy(tword, line+ws, we-ws+1);
            
            word = (char*)malloc(LENGTH+1);
            //if we get back 0, the word doens't have anything but punks
            //  this also clears tword, so we don't need to init it
            //Note, we're stripping the punks BEFORE checking, AND while chekcing,
            //  because the spec defines that check_word() must, but the TEST sample
            //  requires the punks to be stripped from the mspell results
            if(strip_punks(tword, word)==0) { free(word); word=NULL; ws=we; continue; }    //move next if the word is no good
            
            //check the word
            //NOTE:  standalone punks will be flagged as 'misspellings'
            //  And this is why you should NEVER return bool as an error code.
            //  SUCH a horrible practice to promulgate.
            if(!check_word(word,hashtable))
            {
                index = errcount;
                //add to misspelled
                //note:  this will ~never~ be non-null, since we memset at the beginning,
                //  but valgrind complains erroneously about not doing this
                if(misspelled[index]!=NULL) free(misspelled[index]);
                misspelled[index] = word;
                errcount++;
                word=NULL;
            }
            else
            {
                //ugh, i've been programming firmware too long.  I cringe everytime I need to use malloc/free
                //oh, and the word is fine
                free(word);
                word=NULL;
            }

            //if we've reached the max err, just break
            if(errcount>=MAX_MISSPELLED) break;
            ws=we;
        }
        
        //if we've reached the max err, just break
        if(errcount>=MAX_MISSPELLED) break;
    }

    ret = errcount;
    goto EXIT_RET;

EXIT_RET:
    return ret;
}

#ifdef _WIN32
int check_word(const char* word, hashmap_t hashtable[])
#else
bool check_word(const char* word, hashmap_t hashtable[])
#endif
{
    int ret = 0;
    int ps = 0;
    int pe = 0;
    char c;
    int hashloc = 0;
    int i;

    hashmap_t tnode;
    char tword[LENGTH+1];

    //if we get back 0, the word doens't have anything but punks
    if(strip_punks(word, tword)==0) {ret=0; goto EXIT_RET;}

    hashloc = hash_function(tword);
    tnode = hashtable[hashloc];
    //keep going to walk the chain (or no initail match)
    while(tnode!=NULL)
    {
        if(memcmp(tword,tnode->word,LENGTH)==0)  //match
        { ret = 1; goto EXIT_RET; }
        tnode = tnode->next;    //not a match, walk the chain
    }
    
    //since we didn't find it regular, lcase and try again
    for(i=0; i<LENGTH; i++) tword[i] = tolower(tword[i]);
    hashloc = hash_function(tword);
    tnode = hashtable[hashloc];
    //keep going to walk the chain (or no initail match)
    while(tnode!=NULL)
    {
        if(memcmp(tword,tnode->word,LENGTH)==0)  //match
        { ret = 1; goto EXIT_RET; }
        tnode = tnode->next;    //not a match, walk the chain
    }

    //if we get here, there was no match, so false
    ret = 0;

EXIT_RET:
    return ret;
}


//note, this is assuming that a dictionary that contains no good words, but that still loads, is a "successful" load
#ifdef _WIN32
int load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
#else
bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
#endif
{
    int ret = 0;
    int count;
    int i;
    FILE* fp = NULL;
    char c;
    int hashloc;
    int eol;

    char word[LENGTH+1];
    hashmap_t tnode;

    //clear the map
    memset(hashtable, 0, sizeof(hashmap_t)*HASH_SIZE);

    if(dictionary_file==NULL){ret=0; goto EXIT_RET;}

    fp = fopen(dictionary_file, "rb");
    if(!fp) {ret=0; goto EXIT_RET;}

    count = 0;
    //read in all the words
    while(!feof(fp))
    {
        eol = 0;
        memset(word,0,LENGTH+1);    //not really necessary
        //read in the file by char.  once we hit endline, thats a word.
        for(i=0; i<LENGTH; i++)
        {
            c = fgetc(fp);
            if(c==0) break;
            else if(c=='\n' || c=='\r') { eol=1; break; }
            else
            {
                word[i] = c;
            }
        }
        //new word, add it to the hashtable
        //REJECT words that exceed the wordlength max
        if(i>0 && (eol || feof(fp)))
        {
            //note:  this is allocated, and has to be deallocated by the caller.
            //  Valgrind notes this, which is fine, but the top level function (clean()) does
            //  deallocate this.
            tnode = (hashmap_t)malloc(sizeof(node));
            memset(tnode, 0, sizeof(node));
            tnode->next = NULL;
            memcpy(tnode->word, word, i);

            //find where to put it
            hashloc = hash_function(tnode->word)%HASH_SIZE;
            //note: valgrind complains about these indecies.  we have to assume size, sicne we can't modify
            //  to have a proper size accompany the hashtable[] array
            //[Invalid write of size 8]
            if(hashtable[hashloc]==NULL)
            {
                hashtable[hashloc] = tnode;
            }
            else
            {
                //note:  this will never be non-null, but valgrind compalins about it anyway
                if(tnode->next != NULL) free(tnode->next);
                tnode->next = hashtable[hashloc];
                hashtable[hashloc] = tnode;
            }
            count++;
        }
        //we have a word, but we're not at the end of a line (and its not eof)
        else if(i>0 && !eol)
        {
            c=1;
            //chew up the rest of the line
            while(c!=0 && c!='\n' && c!='\r')
            {
                c = fgetc(fp);
            }
        }

    }

    fclose(fp);

    //note, this is assuming that a dictionary that contains no good words, but that still loads, is a "successful" load
    ret = 1;

    
EXIT_RET:
    return ret;
}