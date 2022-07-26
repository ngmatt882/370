		lw		0		1		Five
		sw		0		4		Stack
start	jalr	4		7
		beq		0		1		done
		beq		0		0		start
done	halt
zero    .fill   0
