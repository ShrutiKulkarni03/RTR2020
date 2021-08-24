mkdir -p RoboticArm.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -o RoboticArm.app/Contents/MacOS/RoboticArm RoboticArm.mm -framework Cocoa -framework quartzCore -framework OpenGL
