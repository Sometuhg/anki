# 1. Paths and Flags
RAYLIB_PATH = ./raylib-5.0_linux_amd64
CFLAGS = -Wall -I$(RAYLIB_PATH)/include
LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lm -lpthread -ldl

# 2. The Build Rule
anki: main.c anki.c ui.c
	gcc $(CFLAGS) -o anki main.c anki.c ui.c $(LDFLAGS)

# 3. The Run Rule
run: anki
	LD_LIBRARY_PATH=$(RAYLIB_PATH)/lib ./anki

# 4. Clean Rule (Always good to have)
clean:
	rm -f anki

