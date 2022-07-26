		lw		0		1		Five
		lw		0		4		3
start	jalr	4		7
        sw      0       2       Stack
		beq		0		1		done
		beq		0		0		start
done	halt
Five	.fill	5
        .fill   Stack
