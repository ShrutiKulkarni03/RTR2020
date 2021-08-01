mkdir -p TesselationShader.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o TesselationShader.app/Contents/MacOS/TesselationShader TesselationShader.mm -framework Cocoa -framework quartzCore -framework OpenGL
