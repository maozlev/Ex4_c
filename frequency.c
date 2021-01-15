#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define NUM_LETTERS 26 // ((int)26)

typedef enum {FALSE=0, TRUE=1} boolean;

typedef struct node {
 char letter;
 long unsigned int count;
 boolean end_word;
 boolean is_leaf;
 struct node* children[NUM_LETTERS];
} node;

typedef struct trie {
 node* children[NUM_LETTERS];
 node* current;
 boolean empty;
 int max_length;
 char* word;
} trie;

node* new_node(char letter) {
    node* p;
    int i;

    if (!(p = (node*)malloc(sizeof(node)))) {
        printf("Not enough memory!\n");
        exit(1);
    }

    p->letter = letter;
    p->count = 0;
    p->end_word = FALSE;
    p->is_leaf = FALSE;

    for (i = 0; i < NUM_LETTERS; ++i) {
        (p->children)[i] = NULL;
    }

    return p;
}

void free_node(node* p) {
    int i;
    if(p == NULL) {
        return;
    }
    if(p->is_leaf) {
        for (int i = 0; i <NUM_LETTERS; i++) {
           free_node((p->children)[i]);
        }
    }
    free(p);
    return; 
}

trie* new_trie(trie* t) {
    int i;
    if(!(t=(trie*)malloc(sizeof(trie)))) {
        printf("Not enough memory!\n");
        exit(1);
    }
      
    for (i=0; i<NUM_LETTERS; ++i) {
        t->children[i]=NULL;
    }

    t->current=NULL;
    t->empty=TRUE;
    t->max_length=0;
    t->word=NULL;
    
    return t;
}

boolean is_empty(trie* root) {
    return root->empty;
}

void close_word(trie* root) {
    if (root->current == NULL)
        return;
    root->current->count++;
    root->current->end_word = TRUE;
    root->current = NULL;
}

int char2index(char c) {
    return c - 'a';
}

int read_character(trie* root, int c, int word_length) {
    int index;

    if(!isalpha(c)) {
        if (c == ' ' || c == '\t' || c == '\n' || c== EOF) {
            close_word(root);
            return word_length;
        }
        return word_length;
    }

    word_length++;
    c = tolower(c);
    index = char2index(c);

    if (root->current == NULL) {
        if (root->children[index] == NULL) {
            root->children[index] = new_node(c);
        }
        root->current = root->children[index];
        root->empty = FALSE;
    }
    else {
        root->current->is_leaf = TRUE;
        if (root->current->children[index] == NULL)
            root->current->children[index] = new_node(c);
        root->current=root->current->children[index];
    }
    return word_length;
}

trie* read_text() {
    int c;
    int word_length=0;
    trie* root;
    root = new_trie(root);

    while(EOF != (c=getchar())) {
        word_length = read_character(root, c, word_length);
        if (word_length>root->max_length)
            root->max_length = word_length;
        if(!isalpha(c))
            word_length=0;
    }

   free(root->word);

    if (!(root->word=(char*)malloc(1+sizeof(char)*(root->max_length)))) {
        printf("Not enough memory!\n");
        exit(1);
    }
    return root;
}

void print_words_reverse(trie* root) {
    static int p=0;
    int i;
    node* current;
    root->word[p++] = root->current->letter;

    if (root->current->is_leaf) {
        for (i = NUM_LETTERS - 1; i >= 0; --i) {
            if (root->current->children[i] == NULL)
                continue;
            current = root->current;
            root->current = root->current->children[i];
            print_words_reverse(root);
            root->current = current;
        }
    }
    else {
        if (root->current->end_word) {
            root->word[p] = '\0';
            printf("%s %ld\n",root->word,root->current->count);
        }
        --p;
        return;
    }
    if (root->current->end_word) {
        root->word[p] = '\0';
        printf("%s\t%ld\n",root->word,root->current->count);
    }
    --p;
}

void print_trie_reverse(trie* root) {
    int i;
    if (root == NULL)
        return;
    if (is_empty(root))
        return;

    for (i = NUM_LETTERS - 1; i >= 0; --i) {
        if (root->children[i] == NULL)
            continue;
        root->current = root->children[i];
        print_words_reverse(root);
    }
}

void print_words(trie* root) {
    static int p = 0;
    int i;
    node* current = NULL;
    root->word[p++]=root->current->letter;
    if (root->current->end_word) {
        root->word[p] = '\0';
        printf("%s %ld\n",root->word,root->current->count);
    }
    if (root->current->is_leaf) {
        for( i= 0; i < NUM_LETTERS; ++i) {
            if (root->current->children[i] == NULL)
                continue;
            current = root->current;
            root->current = root->current->children[i];
            print_words(root);
            root->current = current;
        }
    }
    else {
        --p;
        return;
    }
    --p;
}

void print_trie(trie* root) {
    int i;
    if (root == NULL)
        return;
    if (is_empty(root))
        return;

    for (i=0; i<NUM_LETTERS; ++i) {
        if (root->children[i] == NULL)
            continue;
        root->current = root->children[i];
        print_words(root);
    }
}

void free_trie(trie* t) {
    int i;
    if (t == NULL)
        return;
    for(i=0;i<NUM_LETTERS; ++i) {
        free_node(t->children[i]);
    }
    free(t->word);
    free(t);
}

void usage(char* program_name,char* message) {
    printf("\n%s\n\n",message);
    printf("USAGE:\n\t%s: %s\n\n", program_name, message);
    exit(1);
}

boolean should_reverse(int argc, char* argv[]) {
    if (argc > 2)
        usage(argv[0],"Wrong number of arguments");

    if ( (argc == 2) && (argv[1][0]=='r' || argv[1][0]=='R'))
        return TRUE;

    if (argc == 1)
        return FALSE;
    
    usage(argv[0],"Bad command line arguments.");
    return FALSE;
}

int main(int argc, char* argv[]) {
    trie* root;
    boolean rev = should_reverse(argc,argv);
    root = read_text();
    if (rev)
        print_trie_reverse(root);
    else
        print_trie(root);

    free_trie(root);

    return 0;
}

// it was the best of times, it was the worst of times.
// it was the age of wisdom, it was the age of foolishness.
