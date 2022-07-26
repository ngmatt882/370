	    sw  	0	    0       arr
        lw      0       1       one
	    lw	    1       2       arr	    
	    add     1       1       4
        lw      4       3       arr
        add     2       3       5
        add     1       4       4
        sw      4       5       arr
	    halt
one     .fill   1
arr     .fill   7
        .fill   5
        .fill   3
        .fill   9