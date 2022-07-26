start   lw      0       3       Neg1
        add     3       1       1
        sw      0       1       Stack
        lw      0       3       neg2
        nor     3       1       1
        sw      3       1       Stack
        jalr    6       4
File3   .fill   start
neg2    .fill   -2