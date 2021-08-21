mkdir -p Materials.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o Materials.app/Contents/MacOS/Materials Materials.mm -framework Cocoa -framework quartzCore -framework OpenGL
