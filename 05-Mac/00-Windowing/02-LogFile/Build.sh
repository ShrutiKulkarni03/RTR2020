mkdir -p LogFile.app/Contents/MacOS

clang -o LogFile.app/Contents/MacOS/LogFile LogFile.m -framework Cocoa

