        lw      0       1       arrptr
        lw      0       2       arrlen
        lw      0       3       num
        add     0       0       5
loop    add     1       5       6
        lw      6       6       0
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
seven   .fill   7
four    .fill   4
six     .fill   6