# Simple File System Using FUSE

gcc -W fs.c -o fs `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=25 -D_FILE_OFFSET_BITS=64 -lm -g -w <br>
./fs -f /home/(username)/Desktop/mountpoint <br>

Mount point gets created in the Desktop <br>
Open a terminal and type <br>
cd Desktop <br>
cd mountpoint <br>
Open a terminal in the mountpoint and run the Unix commands <br>
