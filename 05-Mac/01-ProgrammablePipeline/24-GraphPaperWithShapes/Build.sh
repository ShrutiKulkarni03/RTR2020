mkdir -p GraphPaperWithShapes.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o GraphPaperWithShapes.app/Contents/MacOS/GraphPaperWithShapes GraphPaperWithShapes.mm -framework Cocoa -framework quartzCore -framework OpenGL
