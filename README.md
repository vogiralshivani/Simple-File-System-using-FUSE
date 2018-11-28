# Simple File System Using FUSE
The objective of the project was to implement and understand the working of a
functional file system for Linux. <br>
The important components for the design of the File System include implementing
system calls to interact with the file system data structure, which is performed using the
operations built for that data structure and be able to achieve persistence(i.e. retain the
data stored on the file system).<br>

In order to mount the file system, run the following commands: <br>
gcc -W fs.c -o fs `pkg-config fuse --cflags --libs` -DFUSE_USE_VERSION=25 -D_FILE_OFFSET_BITS=64 -lm -g -w <br>
./fs -f /home/(username)/Desktop/mountpoint <br>

Mount point gets created in the Desktop <br>
Open a terminal and type <br>
`cd Desktop` <br>
`cd mountpoint` <br>
Open a terminal in the mountpoint and run the Unix commands <br>
