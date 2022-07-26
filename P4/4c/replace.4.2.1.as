        lw      0       1       arrptr
        lw      0       2       arrlen
        lw      0       3       one
        lw      0       7       num
        add     0       0       5       // arr traversal
loop    add     1       5       6
        sw      6       7       0
        lw      6       4       0
        sw      6       4       0
        add     5       3       5
        beq     5       2       end
        beq     0       0       loop
end     halt
num     .fill   100
one     .fill   1
negone  .fill   -1
arrlen  .fill   4
arrptr  .fill   array
array   .fill   0    
seven   .fill   7
four    .fill   4
six     .fill   6
three   .fill   3