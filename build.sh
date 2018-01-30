#!/bin/bash

# -D is like the line in source code: #define BUILD_FOR_LINUX 1 
# -Wall turns on all warnings
# -s strips symbols (reduces size of executable)
# -o2 Nearly all supported optimizations that do not involve a space-speed tradeoff.
# -o specifies the output file. 

# Get folder name. This is onny done for the purposes of giving the executable
# a nicer name, removing it and having the single gcc line (ie removing $dest and 
# putting in whatever you want as the name) is fine
#
# If you have a parameter (any) then attempt to build the main_base.c (if it exists)
# otherwise build main.c

folder=${PWD##*/}

if [[ -z $1 ]]; then
    echo; echo "Building main.c"; echo
    dest="main_$folder"
    dest+="_test"
    gcc ./main.c -DBUILD_FOR_LINUX -Wall -s -o2 -o $dest  
else
    echo; echo "Building main_base.c"; echo
    dest="main_base_$folder"
    dest+="_test"    
    gcc ./main_base.c -DBUILD_FOR_LINUX -Wall -s -o2 -o $dest  
fi




