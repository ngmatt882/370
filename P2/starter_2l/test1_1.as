		lw		0		1		Five
		lw		0		4		SubAdr
start	jalr	4		7
		beq		0		1		done
		beq		0		0		start
done	halt
Five	.fill	50
