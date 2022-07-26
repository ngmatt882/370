        lw      0       1       data
        lw      0       4       File2
        jalr    4       7       //jump to next File2
pause   noop
        sw      0       1       data
        lw      0       2       load
done    halt
data    .fill   3
Neg1    .fill   -1    
load    .fill   Stack  