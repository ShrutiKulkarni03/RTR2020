mkdir -p BnWPyramid.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o BnWPyramid.app/Contents/MacOS/BnWPyramid BnWPyramid.mm -framework Cocoa -framework quartzCore -framework OpenGL
