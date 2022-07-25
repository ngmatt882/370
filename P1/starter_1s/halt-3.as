        lw      0       1       one
        lw      0       7       seven
        lw      0       4       four
        jalr    7       3                 
        halt
        noop
        noop
        noop
        add     0       1       1
        sw      1       1       ten
        jalr    3       6
one     .fill   1
four    .fill   4
seven   .fill   7
ten     .fill   10