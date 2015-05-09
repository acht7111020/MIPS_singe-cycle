	.ORG 0x0
	# memory access
 	addi $1, $0, 0x400 
	addi $2, $0, -0x2BCD 
	sb $2, -1($1)		# store to 1023
	lw $4, 1020($0) 	# $4 = 0xCD000000
	sh $2, -4($1)		# store to 1020
	lw $5, 1020($0)		# $5 = 0xCD00ABCD
	sb $2, -2($1)		# store to 1022
	lw $3, 1020($0)		# $03 = 0xCDCDABCD
	sh $2, -8($1)		# store to 1016
	lw $7, 1016($0)		# $07 = 0x0000ABCD			
	sw $2, -8($1)		# store to 1016
	lw $6, 1016($0)		# $06 = 0xFFFFABCD
	sb $2, -10($1)		# store to 1014
	lw $9, -12($1) 		# $9 = 0x00CD0000
	sh $2, 1014($0)		# store to 1014
	lw $8, -12($1) 		# $8 = 0xABCD0000
	# lb & lbu
	lbu $14, 1023($0)	# $14 = 0x000000CD ff03 0e90
	lb $15, 1023($0)	# $15 = 0xFFFFFFCD ff03 0f80
	# addi & bne & sll
	addi $10, $10, -0x0fff # $10 = 0xFFFF8000 0080 4a21
	addi $11, $0, -0x7ffe # feff 0b20
Back:	
	and $10, $10, $11 	# e457 6a01
	sll $11, $11, 2 	# 4058 eb03
	bne $10, $0, Back 	# fdff 0a14		Back = -3
	# nor & bne & sra
	addi $10, $10, 7	# 0700 4a21
	lui $11, -0x0fff 	# 0080 eb3f 
	halt