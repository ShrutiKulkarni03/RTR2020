mkdir -p DiffuseLightOnCube.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o DiffuseLightOnCube.app/Contents/MacOS/DiffuseLightOnCube DiffuseLightOnCube.mm -framework Cocoa -framework quartzCore -framework OpenGL
