mkdir -p GeometryShader.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o GeometryShader.app/Contents/MacOS/GeometryShader GeometryShader.mm -framework Cocoa -framework quartzCore -framework OpenGL
