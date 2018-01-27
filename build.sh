#!/bin/bash
#gcc ./main.c ./timers.c ./screen.c -DCOMPILE_FOR_LINUX=1 -Wall -s -o2 -o test_timer
gcc ./s2w.c ./timers.c ./platform.c -DCOMPILE_FOR_LINUX=1 -Wall -s -o2 -o sw2


