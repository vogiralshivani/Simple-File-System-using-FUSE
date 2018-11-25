# Simple File System Using FUSE

to compile:
gcc -W fs.c -o fs `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=25 -D_FILE_OFFSET_BITS=64 -lm -g 

to run:
 ./fs -f /home/$(username)/Desktop/mountpoint/
