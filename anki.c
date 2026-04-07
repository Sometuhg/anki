#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "anki.h"

void init_deck(Deck *deck) {
    deck->cards = NULL;
    deck->count = 0;
    deck->capacity = 0;
}

void free_deck(Deck *deck) {
    if (!deck || !deck->cards) return;
    for (size_t i = 0; i < deck->count; i++) {
        free(deck->cards[i].front);
        free(deck->cards[i].back);
    }
    free(deck->cards);
    init_deck(deck);
}

void add_card(Deck *deck, char *front, char *back) {
    if (deck->count == deck->capacity) {
        size_t new_cap = (deck->capacity == 0) ? 4 : deck->capacity * 2;
        Card *temp = realloc(deck->cards, new_cap * sizeof(Card));
        if (!temp) return;
        deck->cards = temp;
        deck->capacity = new_cap;
    }

    char *f = strdup(front);
    char *b = strdup(back);
    if (!f || !b) { free(f); free(b); return; }

    Card c = {
        .front = f,
        .back = b,
        .due_date = time(NULL),
        .reps = 0,
        .interval = 0,
        .ease_factor = 2.5f
    };

    deck->cards[deck->count++] = c;
}

void delete_card(Deck *deck, size_t index) {
    if (index >= deck->count) return;
    free(deck->cards[index].front);
    free(deck->cards[index].back);
    for (size_t i = index; i < deck->count - 1; i++) {
        deck->cards[i] = deck->cards[i + 1];
    }
    deck->count--;
}

void sm2_update(Card *card, int rating) {
    float adj = 0.1f - (3 - rating) * (0.08f + (3 - rating) * 0.02f);
    card->ease_factor += adj;
    if (card->ease_factor < 1.3f) card->ease_factor = 1.3f;

    if (rating == 1) {
        card->reps = 0;
        card->interval = 1;
    } else {
        if (card->reps == 0) card->interval = 1;
        else if (card->reps == 1) card->interval = 6;
        else card->interval = (int)round(card->interval * card->ease_factor);
        card->reps++;
    }
    card->due_date = time(NULL) + (card->interval * 86400);
}

int save_deck(const Deck *deck, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    for (size_t i = 0; i < deck->count; i++) {
        Card c = deck->cards[i];
        fprintf(f, "%s|%s|%ld|%d|%d|%f\n", c.front, c.back, (long)c.due_date, c.reps, c.interval, c.ease_factor);
    }
    fclose(f);
    return 0;
}

int load_deck(Deck *deck, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char buf[1024]; 
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n")] = 0;
        char *front = strtok(buf, "|"), *back = strtok(NULL, "|"), *date = strtok(NULL, "|");
        char *reps = strtok(NULL, "|"), *inter = strtok(NULL, "|"), *ease = strtok(NULL, "|");
        if (front && back && date && reps && inter && ease) {
            add_card(deck, front, back);
            Card *c = &deck->cards[deck->count - 1];
            c->due_date = (time_t)atol(date);
            c->reps = atoi(reps);
            c->interval = atoi(inter);
            c->ease_factor = (float)atof(ease);
        }
    }
    fclose(f);
    return 0;
}

void review_deck(Deck *deck) {
    time_t now = time(NULL);
    int count = 0;
    for (size_t i = 0; i < deck->count; i++) {
        Card *c = &deck->cards[i];
        if (c->due_date <= now) {
            count++;
            printf("\nQ: %s\n[Enter] to see A...", c->front);
            while(getchar() != '\n');
            printf("A: %s\nRating (1:Hard, 2:Mid, 3:Easy): ", c->back);
            int r = 0;
            while (scanf("%d", &r) != 1 || r < 1 || r > 3) {
                while(getchar() != '\n');
                printf("Invalid. 1-3: ");
            }
            while(getchar() != '\n');
            sm2_update(c, r);
            printf("Next review in %d days.\n", c->interval);
        }
    }
    if (!count) printf("\nNo cards due!\n");
}

int get_due_count(const Deck *deck) {
    int count = 0;
    time_t now = time(NULL);
    
    for (size_t i = 0; i < deck->count; i++) {
        if (deck->cards[i].due_date <= now) {
            count++;
        }
    }
    return count;
}
