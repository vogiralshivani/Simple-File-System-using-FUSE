# Simple File System Using FUSE

gcc -W fs.c -o fs `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=25 -D_FILE_OFFSET_BITS=64 -lm -g -w
./fs -f /home/(username)/Desktop/mountpoint 

Mount point gets created in the desktop
Open a terminal and type
cd Desktop
cd mountpoint
Open a terminal in the mountpoint and run the Unix commands
