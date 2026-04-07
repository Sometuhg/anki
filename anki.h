#ifndef ANKI_H
#define ANKI_H

#include <time.h>
#include <stddef.h>

// 1. Data Structures
typedef struct {
    char *front;
    char *back;
    time_t due_date;
    int reps;
    int interval;
    float ease_factor;
} Card;

typedef struct {
    Card *cards;
    size_t count;
    size_t capacity;
} Deck;

// 2. Core Function Signatures
void init_deck(Deck *deck);
void free_deck(Deck *deck);
void add_card(Deck *deck, char *front, char *back);
void delete_card(Deck *deck, size_t index);

// 3. Algorithm & Logic
void sm2_update(Card *card, int rating);
void review_deck(Deck *deck);

// 4. Persistence
int save_deck(const Deck *deck, const char *path);
int load_deck(Deck *deck, const char *path);

int get_due_count(const Deck *deck);


#endif

