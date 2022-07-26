bonk    lw      0       1       bonk
        lw      5       2       yoink
        beq     4       3       -32768
        sw      2       7       40000
        add     2       3       4
        lw      2       3       Whee
        noop
end     halt
honk    .fill   -79     
yoink   .fill   32768
Whee    .fill   32800