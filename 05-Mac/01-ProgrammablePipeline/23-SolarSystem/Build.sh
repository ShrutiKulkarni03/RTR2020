mkdir -p SolarSystem.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o SolarSystem.app/Contents/MacOS/SolarSystem SolarSystem.mm -framework Cocoa -framework quartzCore -framework OpenGL
