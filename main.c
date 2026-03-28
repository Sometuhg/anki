#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *front;
    char *back;
    time_t due_date;
    int difficulty;

    int interval;
    int repetition;
} Card;

typedef struct {
    Card *cards;
    size_t count;
    size_t capacity;
} Deck;


void add_card(Deck *deck, char *front, char *back) {
    if (deck->count == deck->capacity) {
        size_t new_capacity = (deck->capacity == 0) ? 4 : deck->capacity * 2;
        Card *temp = realloc(deck->cards, new_capacity * sizeof(Card));
        if (temp == NULL) return;

        deck->cards = temp;
        deck->capacity = new_capacity;
    }

    char *new_front = strdup(front);
    if (new_front == NULL) return;

    char *new_back = strdup(back);
    if (new_back == NULL) {
        free(new_front);
        return;
    }

    Card new_card;
    new_card.front = new_front;
    new_card.back = new_back;
    new_card.due_date = time(NULL);
    new_card.difficulty = 1;

    deck->cards[deck->count] = new_card;
    deck->count++;
}

void init_deck(Deck *deck) {
    deck->cards = NULL;
    deck->count = 0;
    deck->capacity = 0;
}

void free_deck(Deck *deck) {
    if (deck == NULL || deck->cards == NULL) return;

    for (size_t i = 0; i < deck->count; i++) {
        free(deck->cards[i].front);
        free(deck->cards[i].back);
    }

    free(deck->cards);

    deck->cards = NULL;
    deck->count = 0;
    deck->capacity = 0;
}

int save_deck(const Deck *deck, const char *filename) {
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        return -1; 
    }

    for (size_t i = 0; i < deck->count; i++) {
        Card c = deck->cards[i];

        fprintf(file, "%s|%s|%d|%ld\n", 
                c.front, 
                c.back, 
                c.difficulty, 
                (long)c.due_date);
    }

    fclose(file);
    return 0;
}

void review_deck(Deck *deck) {
    time_t now = time(NULL);
    int due_count = 0;

    printf("\n Starting Review Session \n");

    for (size_t i = 0; i < deck->count; i++) {
        Card *c = &deck->cards[i];

        // Only show cards if the due_date is NOW or in the PAST
        if (c->due_date <= now) {
            due_count++;
            
            printf("\nQUESTION: %s\n", c->front);
            printf("Press Enter to see answer...");
            getchar(); // Wait for user to hit enter

            printf("ANSWER:   %s\n", c->back);
        

	   // get rating
	   int rating = 0;
	   while (rating < 1 || rating > 3) {
    	   printf("\nRate (1: Hard, 2: Mid, 3: Easy): ");
    
	   if (scanf("%d", &rating) != 1) {
            //  clear the garbage
        	while (getchar() != '\n'); 
        	printf("Please enter a digit (1-3).\n");
        	continue;
     	   }
    	   getchar(); // Eat the left over garbage just to be safe
   	      if (rating < 1 || rating > 3) {
              printf("Invalid choice. Pick 1, 2, or 3.\n");
   	   }
	 } 

            // Update the Due Date (86400 seconds = 1 day)
            if (rating == 1) {
                c->due_date = now + 86400;      // +1 Day
            } else if (rating == 2) {
                c->due_date = now + (86400 * 3); // +3 Days
            } else {
                c->due_date = now + (86400 * 7); // +7 Days
            }

            printf("Card scheduled for later!\n");
        }
    }

    if (due_count == 0) {
        printf("All caught up! No cards due for review.\n");
    }
}


int load_deck(Deck *deck, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return -1;

    char buffer[1024]; // the bucket for 1 line, 1024 is probably wasteful here
    
    while (fgets(buffer, sizeof(buffer), file)) {
        char *front_part = strtok(buffer, "|");
        
        char *back_part = strtok(NULL, "|");
        
        char *diff_part = strtok(NULL, "|");
        
        char *date_part = strtok(NULL, "|");

        // If the line was malformed, skip the card
        if (!front_part || !back_part || !diff_part || !date_part) continue;

        add_card(deck, front_part, back_part);
        
        Card *newest = &deck->cards[deck->count - 1];
        newest->difficulty = atoi(diff_part);
        newest->due_date = (time_t)atol(date_part);
    }

    fclose(file);
    return 0;
}


int main() {
    Deck my_deck;
    init_deck(&my_deck);

    if (load_deck(&my_deck, "deck.txt") == 0) {
        printf("Loaded %zu cards.\n", my_deck.count);
    }

    int choice = 0;
    while (choice != 3) {
        printf("\n FLASHCARD MENU \n");
        printf("1. Add a New Card\n");
        printf("2. Review Due Cards\n");
        printf("3. Save and Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear trash
            continue;
        }
        getchar(); // Eat the newline from Enter

        if (choice == 1) {
            char front[256], back[256];
            printf("Front: ");
            fgets(front, sizeof(front), stdin);
            front[strcspn(front, "\n")] = 0;

            printf("Back:  ");
            fgets(back, sizeof(back), stdin);
            back[strcspn(back, "\n")] = 0;

            add_card(&my_deck, front, back);
            printf("Card added!\n");

        } else if (choice == 2) {
            review_deck(&my_deck);

        } else if (choice == 3) {
            if (save_deck(&my_deck, "deck.txt") == 0) {
                printf("Progress saved to deck.txt.\n");
            }
            printf("Goodbye!\n");
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    // Cleanup: Wipe the heap
    free_deck(&my_deck);
    return 0;
}

