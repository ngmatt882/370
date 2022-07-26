        lw      0       2       mcand
        lw      0       3       mplier
        lw      0       5       one     // r5 = 1
        lw      0       7       one     // r7 = 1
        nor     3       3       3
start   nor     7       7       7       // nor r7
        nor     7       3       4       // r4 store bits
        beq     0       4       skip    // if 0 do nothing
        add     1       2       1       // result = result + reg2
skip    lw      0       4       limit   // r4 is now 15
        beq     5       4       done    // if counter is 15 halt
        add     2       2       2       // double reg2
        nor     7       7       7       // nor reg7
        add     7       7       7       // double reg7
        lw      0       6       one     // r6 = 1
        add     6       5       5       // ++r5
        beq     0       0       start   // else loop back    
done    halt
one     .fill   1
limit   .fill   15
mcand   .fill   1103
mplier  .fill   7043