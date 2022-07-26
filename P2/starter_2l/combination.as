start   sw          5       7       Stack       // store return address
        lw          0       6       one         // r6 = 1; for incrementing
        add         5       6       5           // ++Stack
        sw          5       1       Stack       // store n
        add         5       6       5           // ++Stack
        sw          5       2       Stack       // store r
        add         5       6       5           // ++Stack, needs to prep the next jumps
        beq         1       2       cases       // if n = r, jump
        beq         0       2       cases       // if r = 0, jump
        add         6       6       6           // ++r6
        beq         6       2       tri         // if r = 2, jump to triangular formula
        lw          0       6       neg2        // r6 = -2
        add         6       1       4           // r4 = r-2
        beq         4       2       tri         // if r = n-2, jump to triangular formula     
n1      lw          0       6       neg1        // r6 = -1, make n-1
        add         1       6       1           // r1 = n-1
        lw          0       4       Caddr       // jump to cases
        jalr        4       7                   // computing n-1, r
n1r1    lw          0       6       neg1        // r6 = -1, make r-1
        add         2       6       2           // r2 = r-1
        lw          0       4       Caddr       // jump to cases
        jalr        4       7                   // computing n-1, r-1
sum     lw          0       6       neg1        // r6 = -1, decrement Stack
        add         5       6       5           // --Stack
        lw          5       3       Stack       // load second return value
        add         5       6       5           // --Stack
        lw          5       6       Stack       // load first return value
        add         3       6       3           // add return values together
return  lw          0       6       neg1        // r6 = -1, decrement Stack
        add         6       5       5           // --Stack, need to -1 since empty
        lw          5       2       Stack       // r2 = whatever r is on Stack
        add         6       5       5           // --Stack
        lw          5       1       Stack       // r1 = whatever n is on Stack
        add         6       5       5           // --Stack
        lw          5       7       Stack       // r7 = whatever return address is on Stack
        sw          5       3       Stack       // store "answer", replaces return address
        lw          0       6       one
        add         5       6       5           // ++Stack       
        jalr        7       4                   // jump to original address
cases   lw          0       3       one         // return 1 for cases
        beq         0       0       return
tri     lw          0       6       one         // r6 = 1
        lw          0       4       neg1        // r4 = -1
        add         1       4       1           // --n
        lw          0       4       one         // r4 = 1
        add         0       1       3           // r3 = n
begin   add         1       3       3           // r3 = r3 + n
        add         4       6       6           // ++r6
        beq         1       6       skip        // done squaring
        beq         0       0       begin
skip    add         3       1       3           // n^2 + n
        lw          0       6       neg2        // r6 = -2
        add         0       0       4
jump    add         3       6       3
        lw          0       7       one
        add         4       7       4 
        beq         0       3       next
        beq         0       0       jump
next    add         0       4       3       
done    beq         0       0       return      // jump to return
Caddr   .fill       start
one     .fill       1
two     .fill       2
neg1    .fill       -1
neg2    .fill       -2
