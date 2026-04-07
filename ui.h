#ifndef UI_H
#define UI_H
#include "raylib.h"
#include "anki.h"
#include <stdbool.h>

//  Scene Tracking
typedef enum {
    SCENE_DASHBOARD = 0,
    SCENE_REVIEW,
    SCENE_LIBRARY,
    SCENE_EDITOR
} AppScene;

// Global Application State
typedef struct {
    Deck deck;
    AppScene currentScene;
    
    // Review Logic
    int currentCardIdx;     // Index of the card currently being shown
    bool isFlipped;         // Toggle between Front and Back view
    
    // Input Buffers for Editor
    Font font;
    char frontBuf[256];
    char backBuf[256];
    int frontLen;       // Current cursor position for front
    int backLen;        // Current cursor position for back
    int activeField;
} AppState;

//  Entry Point
// This will initialize Raylib, run the game loop, and cleanup on exit.
void run_ui(void);

#endif

