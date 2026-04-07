#include "raylib.h"
#include "ui.h"
#include "anki.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


float GetNextWordWidth(Font font, const char* text, int startIndex, float fontSize, float spacing) {
    float width = 0;
    int i = startIndex;
    while (text[i] != '\0' && text[i] != ' ' && text[i] != '\n') {
        char c[2] = { text[i], '\0' };
        width += MeasureTextEx(font, c, fontSize, spacing).x + spacing;
        i++;
    }
    return width;
}

Vector2 GetWrappedCursorPos(Font font, const char *text, Rectangle bounds, float fontSize) {
    float spacing = 2.0f;
    float lineSpacing = 5.0f;
    Vector2 cursor = { bounds.x, bounds.y };

    for (int i = 0; i < (int)strlen(text); i++) {
        if (text[i] == '\n') {
            cursor.x = bounds.x;
            cursor.y += fontSize + lineSpacing;
            continue;
        }

        if (text[i] == ' ') {
            float wordWidth = GetNextWordWidth(font, text, i + 1, fontSize, spacing);
            float spaceWidth = MeasureTextEx(font, " ", fontSize, spacing).x + spacing;
            if (cursor.x + spaceWidth + wordWidth > bounds.x + bounds.width) {
                cursor.x = bounds.x;
                cursor.y += fontSize + lineSpacing;
                continue;
            }
        }

        char character[2] = { text[i], '\0' };
        Vector2 size = MeasureTextEx(font, character, fontSize, spacing);
        if (cursor.x + size.x > bounds.x + bounds.width) {
            cursor.x = bounds.x;
            cursor.y += fontSize + lineSpacing;
        }
        cursor.x += size.x + spacing;
    }
    return cursor;
}

float MeasureTextWrappedHeight(Font font, const char *text, float width, float fontSize) {
    float spacing = 2.0f;
    float lineSpacing = 5.0f;
    Vector2 cursor = { 0, 0 };
    float totalHeight = fontSize; 

    for (int i = 0; i < (int)strlen(text); i++) {
        if (text[i] == '\n') {
            cursor.x = 0;
            cursor.y += fontSize + lineSpacing;
            totalHeight = cursor.y + fontSize;
            continue;
        }

        if (text[i] == ' ') {
            float wordWidth = GetNextWordWidth(font, text, i + 1, fontSize, spacing);
            float spaceWidth = MeasureTextEx(font, " ", fontSize, spacing).x + spacing;
            if (cursor.x + spaceWidth + wordWidth > width) {
                cursor.x = 0;
                cursor.y += fontSize + lineSpacing;
                totalHeight = cursor.y + fontSize;
                continue;
            }
        }

        char character[2] = { text[i], '\0' };
        Vector2 size = MeasureTextEx(font, character, fontSize, spacing);
        if (cursor.x + size.x > width) {
            cursor.x = 0;
            cursor.y += fontSize + lineSpacing;
            totalHeight = cursor.y + fontSize;
        }
        cursor.x += size.x + spacing;
    }
    return totalHeight;
}

void DrawTextWrapped(Font font, const char *text, Rectangle bounds, float fontSize, Color color) {
    float spacing = 2.0f;
    float lineSpacing = 5.0f;
    Vector2 cursor = { bounds.x, bounds.y };

    for (int i = 0; i < (int)strlen(text); i++) {
        if (text[i] == '\n') {
            cursor.x = bounds.x;
            cursor.y += fontSize + lineSpacing;
            continue;
        }

        if (text[i] == ' ') {
            float wordWidth = GetNextWordWidth(font, text, i + 1, fontSize, spacing);
            float spaceWidth = MeasureTextEx(font, " ", fontSize, spacing).x + spacing;
            if (cursor.x + spaceWidth + wordWidth > bounds.x + bounds.width) {
                cursor.x = bounds.x;
                cursor.y += fontSize + lineSpacing;
                continue;
            }
        }

        char character[2] = { text[i], '\0' };
        Vector2 size = MeasureTextEx(font, character, fontSize, spacing);
        if (cursor.x + size.x > bounds.x + bounds.width) {
            cursor.x = bounds.x;
            cursor.y += fontSize + lineSpacing;
        }
        DrawTextEx(font, character, cursor, fontSize, spacing, color);
        cursor.x += size.x + spacing;
    }
}

bool DrawButton(Rectangle rect, const char *text, Font font, Color baseColor) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);
    Color drawColor = hover ? ColorBrightness(baseColor, 0.2f) : baseColor;

    DrawRectangleRec(rect, drawColor);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
    
    float fontSize = 24.0f;
    float spacing = 2.0f;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 textPos = { rect.x + (rect.width/2 - textSize.x/2), rect.y + (rect.height/2 - textSize.y/2) };
    
    DrawTextEx(font, text, textPos, fontSize, spacing, WHITE);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void draw_editor_box(Rectangle rect, char *buf, Font font, bool active) {
    DrawRectangleRec(rect, active ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx(rect, 2, active ? BLUE : GRAY);

    BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
        Rectangle padding = { rect.x + 10, rect.y + 10, rect.width - 20, rect.height - 20 };
        float totalHeight = MeasureTextWrappedHeight(font, buf, padding.width, 24.0f);
        
        if (totalHeight > padding.height) padding.y -= (totalHeight - padding.height);

        DrawTextWrapped(font, buf, padding, 24.0f, BLACK);

        if (active && (((int)(GetTime() * 2)) % 2 == 0)) {
            Vector2 cursorCoords = GetWrappedCursorPos(font, buf, padding, 24.0f);
            DrawRectangle(cursorCoords.x, cursorCoords.y, 2, 20, BLACK);
        }
    EndScissorMode();
}

// SCENES for my sanity

void draw_dashboard(AppState *state) {
    int due = get_due_count(&state->deck);
    DrawTextEx(state->font, TextFormat("Due Today: %d", due), (Vector2){ 300, 120 }, 24, 2, (due > 0) ? RED : DARKGRAY);
    DrawTextEx(state->font, TextFormat("Total Library: %d", (int)state->deck.count), (Vector2){ 300, 160 }, 20, 2, GRAY);

    if (DrawButton((Rectangle){ 300, 220, 200, 60 }, "START REVIEW", state->font, (due > 0) ? BLUE : GRAY)) {
        if (due > 0) { state->currentScene = SCENE_REVIEW; state->currentCardIdx = 0; state->isFlipped = false; }
    }
    if (DrawButton((Rectangle){ 300, 300, 200, 45 }, "Library", state->font, DARKGRAY)) state->currentScene = SCENE_LIBRARY;
    if (DrawButton((Rectangle){ 300, 360, 200, 45 }, "Add Card", state->font, DARKGRAY)) { state->currentScene = SCENE_EDITOR; state->activeField = 1; }
}

void draw_library(AppState *state) {
    DrawTextEx(state->font, "Card Library", (Vector2){ 330, 40 }, 24, 2, DARKGRAY);
    for (size_t i = 0; i < state->deck.count && i < 8; i++) {
        float y = 100 + (i * 50);
        Rectangle row = { 100, y, 600, 40 };
        DrawRectangleRec(row, RAYWHITE);
        DrawRectangleLinesEx(row, 1, LIGHTGRAY);
        DrawTextEx(state->font, state->deck.cards[i].front, (Vector2){ row.x + 10, row.y + 10 }, 18, 2, BLACK);
        if (DrawButton((Rectangle){ row.x + 540, row.y + 5, 50, 30 }, "DEL", state->font, MAROON)) { delete_card(&state->deck, i); break; }
    }
    if (DrawButton((Rectangle){ 325, 520, 150, 45 }, "BACK", state->font, GRAY) || IsKeyPressed(KEY_ESCAPE)) state->currentScene = SCENE_DASHBOARD;
}

void draw_review(AppState *state) {
    static int lastIdx = -1;
    static float qScroll = 0.0f, aScroll = 0.0f;
    Card *c = NULL;
    int found_idx = -1;
    for (size_t i = state->currentCardIdx; i < state->deck.count; i++) {
        if (state->deck.cards[i].due_date <= (long)time(NULL)) { c = &state->deck.cards[i]; found_idx = (int)i; break; }
    }
    if (!c) {
        DrawTextEx(state->font, "Session Finished!", (Vector2){ 310, 250 }, 24, 2, DARKGRAY);
        if (DrawButton((Rectangle){ 325, 300, 150, 40 }, "BACK HOME", state->font, GRAY)) state->currentScene = SCENE_DASHBOARD;
        return;
    }
    if (state->currentCardIdx != lastIdx) { qScroll = aScroll = 0; lastIdx = state->currentCardIdx; }

    Rectangle cardRect = { 150, 60, 500, 360 };
    DrawRectangleRec(cardRect, RAYWHITE); DrawRectangleLinesEx(cardRect, 3, LIGHTGRAY);
    DrawTextEx(state->font, "QUESTION:", (Vector2){ cardRect.x + 20, cardRect.y + 15 }, 18, 2, GRAY);
    
    Rectangle qBounds = { cardRect.x + 30, cardRect.y + 45, 440, 130 };
    float qTextH = MeasureTextWrappedHeight(state->font, c->front, qBounds.width, 22.0f);
    if (qTextH > qBounds.height && CheckCollisionPointRec(GetMousePosition(), qBounds)) qScroll += GetMouseWheelMove() * 20.0f;
    qScroll = fmaxf(fminf(qScroll, 0), -(qTextH - qBounds.height > 0 ? qTextH - qBounds.height : 0));

    BeginScissorMode((int)qBounds.x, (int)qBounds.y, (int)qBounds.width, (int)qBounds.height);
        Rectangle qDraw = qBounds; qDraw.y += qScroll; DrawTextWrapped(state->font, c->front, qDraw, 22, BLACK);
    EndScissorMode();

    if (state->isFlipped) {
        DrawLine(cardRect.x + 20, cardRect.y + 185, cardRect.x + 480, cardRect.y + 185, LIGHTGRAY);
        DrawTextEx(state->font, "ANSWER:", (Vector2){ cardRect.x + 20, cardRect.y + 195 }, 18, 2, GRAY);
        Rectangle aBounds = { cardRect.x + 30, cardRect.y + 220, 440, 130 };
        float aTextH = MeasureTextWrappedHeight(state->font, c->back, aBounds.width, 22.0f);
        if (aTextH > aBounds.height && CheckCollisionPointRec(GetMousePosition(), aBounds)) aScroll += GetMouseWheelMove() * 20.0f;
        aScroll = fmaxf(fminf(aScroll, 0), -(aTextH - aBounds.height > 0 ? aTextH - aBounds.height : 0));

        BeginScissorMode((int)aBounds.x, (int)aBounds.y, (int)aBounds.width, (int)aBounds.height);
            Rectangle aDraw = aBounds; aDraw.y += aScroll; DrawTextWrapped(state->font, c->back, aDraw, 22, DARKBLUE);
        EndScissorMode();

        if (DrawButton((Rectangle){ 180, 440, 120, 45 }, "1: Hard", state->font, MAROON)) { sm2_update(c, 1); state->isFlipped = false; state->currentCardIdx = found_idx + 1; }
        if (DrawButton((Rectangle){ 340, 440, 120, 45 }, "2: Mid", state->font, ORANGE)) { sm2_update(c, 2); state->isFlipped = false; state->currentCardIdx = found_idx + 1; }
        if (DrawButton((Rectangle){ 500, 440, 120, 45 }, "3: Easy", state->font, DARKGREEN)) { sm2_update(c, 3); state->isFlipped = false; state->currentCardIdx = found_idx + 1; }
    } else if (DrawButton((Rectangle){ 300, 440, 200, 50 }, "SHOW ANSWER", state->font, BLUE) || IsKeyPressed(KEY_SPACE)) state->isFlipped = true;
}

void draw_editor(AppState *state) {
    Rectangle fRect = { 200, 120, 400, 80 }, bRect = { 200, 250, 400, 80 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), fRect)) state->activeField = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), bRect)) state->activeField = 2;
        else state->activeField = 0;
    }

    if (state->activeField > 0) {
	
        char *buf = (state->activeField == 1) ? state->frontBuf : state->backBuf;
        int *len = (state->activeField == 1) ? &state->frontLen : &state->backLen;
        int key = GetCharPressed();

	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
   	 const char *clipboard = GetClipboardText();
    
    	 if (clipboard != NULL) {
        int clipLen = (int)strlen(clipboard);
        
        for (int i = 0; i < clipLen && *len < 255; i++) {
            char c = clipboard[i];
            
            // SANITIZATION: 
            //  Block the pipe character '|'
            //  Allow standard printable characters (32-126) OR Newlines (\n)
            if (c != '|' && ((c >= 32 && c <= 126) || c == '\n')) {
                buf[*len] = c;
                (*len)++;
            		}
        	}
        	buf[*len] = '\0'; // Always cap it off
   	 	}
	}

        while (key > 0) { if (key >= 32 && key <= 125 && *len < 255) { buf[*len] = (char)	key; buf[++(*len)] = '\0'; } key = GetCharPressed(); }
        if (IsKeyPressed(KEY_BACKSPACE) && *len > 0) buf[--(*len)] = '\0';
        if (IsKeyPressed(KEY_ENTER) && *len < 255) { buf[*len] = '\n'; buf[++(*len)] = '\0'; }
    }

    DrawTextEx(state->font, "Card Editor", (Vector2){ 330, 50 }, 24, 2, DARKGRAY);
    draw_editor_box(fRect, state->frontBuf, state->font, state->activeField == 1);
    draw_editor_box(bRect, state->backBuf, state->font, state->activeField == 2);

    if (DrawButton((Rectangle){ 250, 380, 130, 45 }, "SAVE", state->font, DARKGREEN)) {
        if (state->frontLen > 0 && state->backLen > 0) {
            add_card(&state->deck, state->frontBuf, state->backBuf);
            memset(state->frontBuf, 0, 256); memset(state->backBuf, 0, 256);
            state->frontLen = state->backLen = state->activeField = 0; state->currentScene = SCENE_DASHBOARD;
        }
    }
    if (DrawButton((Rectangle){ 420, 380, 130, 45 }, "CANCEL", state->font, MAROON)) state->currentScene = SCENE_DASHBOARD;
}


void run_ui(void) {
    InitWindow(800, 600, "Anki-C");
    SetTargetFPS(60);

    AppState state = { 0 };
    
    //  Load the Custom Font into the state
    state.font = LoadFontEx("Inter-Regular.ttf", 64, 0, 128); 
    
    // This should make it sharper who knows
    SetTextureFilter(state.font.texture, TEXTURE_FILTER_POINT);

    //Initialize Deck logic
    init_deck(&state.deck);
    load_deck(&state.deck, "deck.txt");
    state.currentScene = SCENE_DASHBOARD;

    // windowing and stuff
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE); 

            switch (state.currentScene) {
                case SCENE_DASHBOARD: draw_dashboard(&state); break;
                case SCENE_REVIEW:    draw_review(&state);    break;
                case SCENE_EDITOR:    draw_editor(&state);    break;
                case SCENE_LIBRARY:   draw_library(&state);   break;
            }
        EndDrawing();
    }

    save_deck(&state.deck, "deck.txt");
    free_deck(&state.deck);
    
    UnloadFont(state.font); 
    
    CloseWindow();
}
