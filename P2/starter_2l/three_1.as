start   lw      0       2       neg1
        add     2       1       1
        sw      0       1       Stack
        lw      0       4       File3
        jalr    4       6       //jump to files 3
        jalr    7       6       //jump back to file 1
neg1    .fill   -1
File2   .fill   start