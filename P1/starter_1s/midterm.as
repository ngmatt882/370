        lw      0       1       arrptr
        lw      0       2       arrlen
        lw      0       3       num
        add     0       0       5
loop    lw      1       6       5
        beq     6       3       end
        lw      0       6       one
        add     5       6       5
        beq     2       5       done
        beq     0       0       loop
done    lw      0       5       negone
end     halt
num     .fill   6
one     .fill   1
negone  .fill   -1
arrlen  .fill   4
arrptr  .fill   array
array   .fill   0    
seven   .fill   3
four    .fill   6
six     .fill   2