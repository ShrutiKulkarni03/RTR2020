mkdir -p TweakedSmiley.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o TweakedSmiley.app/Contents/MacOS/TweakedSmiley TweakedSmiley.mm -framework Cocoa -framework quartzCore -framework OpenGL
