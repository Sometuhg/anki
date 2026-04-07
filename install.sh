#!/bin/bash


mkdir -p ~/.local/bin
mkdir -p ~/.local/share/anki-c
mkdir -p ~/.local/share/anki-c
cp anki ~/.local/share/anki-c/
cp Inter-Regular.ttf ~/.local/share/anki-c/
cp raylib-5.0_linux_amd64/lib/libraylib.so.500 ~/.local/share/anki-c/

cat > ~/.local/bin/anki-c << 'EOF'
#!/bin/bash
cd ~/.local/share/anki-c
LD_LIBRARY_PATH=~/.local/share/anki-c ./anki
Exec=/home/cornsyrup/.local/share/anki-c/anki-c
EOF

chmod +x ~/.local/bin/anki-c
echo "Done! Run with: anki-c"
