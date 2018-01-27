#!/bin/bash

# -D is like the line in source code: #define BUILD_FOR_LINUX 1 
# -Wall turns on all warnings
# -s strips symbols (reduces size of executable)
# -o2 Nearly all supported optimizations that do not involve a space-speed tradeoff.
# -o specifies the output file. 

# Get folder name. This is onny done for the purposes of giving the executable
# a nicer name, removing it and having the single gcc line (ie removing $dest and 
# putting in whatever you want as the name) is fine

folder=${PWD##*/}
dest="main_$folder"
dest+="_test"
#echo $dest

gcc ./main.c -DBUILD_FOR_LINUX -Wall -s -o2 -o $dest  


