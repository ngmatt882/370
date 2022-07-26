        sw      0       5       0                   //stack ptr = 0
        lw      0       4       start               //store start location in reg 4
start   sw      5       1       Stack       //n to stack
        lw      0       6       one                 //load 1 to temp
        add     5       6       5                   //stack++
        sw      5       2       Stack       //r to stack
        add     5       6       5                   //stack++
        sw      5       7       Stack       //return address to stack
        add     5       6       5                   //stack++
        beq     1       2       ret1        //base cases
        beq     2       0       ret1        //^
        lw      0       6       neg1                //load -1 to temp
        add     1       6       1           // n = n-1
        jalr    4       7                           //recursive call
a1      sw      5       3       Stack       //store result (first) to stack
        lw      0       6       one                 //load 1 to temp
        add     5       6       5                   //stack++
        lw      0       6       neg1                //load -1 to temp
        add     1       6       1           // n = n-1
        add     2       6       2           // r = r-1
        jalr    4       7                           //recursive call
a2      lw      0       6       one                 //load 1 to temp
        add     5       6       5                   //stack++
        sw      5       3       Stack       //store result (second) to stack
        lw      5       6       Stack       //temp = stack (second)
        lw      0       6       neg1                //load -1 to temp
        add     5       6       5                   //stack--
        lw      5       3       Stack       //load stack (first) to return
        add     3       6       3           //return = temp(second) + return(first)
        beq     0       0       Lprev       //return sum
ret1    lw      0       3       one         //return 1
Lprev   lw      0       6       neg1                //load -1 to temp
        add     5       6       5                   //stack--
        lw      5       7       Stack       //load return address
        lw      0       6       a1                  //temp = a1
        beq     7       6       cont        //if return address is a1(not a2): skip extra stack--
        add     5       6       5                   //stack--
cont    add     5       6       5                   //stack--
        add     5       6       5                   //stack--
        add     5       6       5                   //stack--
        add     5       6       5                   //stack--
        lw      5       2       Stack       //load prev r
        add     5       6       5                   //stack--
        lw      5       1       Stack       //load prev n
        lw      0       6       one                 //load 1 to temp
        add     5       6       5                   //stack++
        add     5       6       5                   //stack++
        add     5       6       5                   //stack++
        beq     0       0       7           //stackptr restored, jump to location
Caddr   .fill   start
one     .fill   1
neg1    .fill   -1