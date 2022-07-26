bonk    lw      0       1       bonk
yoink   lw      5       2       yoink
        add     2       3       4
Whee    noop
        noop
Whee    add     1       1       1
end     beq     2       4       Whee
end     halt
bonk    .fill   -79     
yoink   .fill   75