	.ORG 0x0 # $sp = 0x03
	sll $0, $0, 0 		# can't detect write $0 error
	lw  $0, 1($sp)		# write to $0 error
	# shift part
	lw $1, 0($0)		# $1 = 0x8000000    data at 0
	addi $1, $1, -1		# number overflow 
	bne $0, $31, 3 		# see whether ($0 == 0)? 
	srl $1, $1, 8 		# $1 = 0x00800000
	srl $1, $1, 8 		# $1 = 0x00008000
	sll $1, $1, 16 		# $1 = 0x80000000
	sra $0, $0, 0 		# write $0 error
	sra $1, $1, 31 		# $1 = 0xFFFFFFF
	addi $1, $1, 1 		# $1 = 0 
	# number overflow
	lw   $1,  1($sp)	# $1=0x7FFFFFFF     data at 4
	addi $0,  $1, 1 	# write to $0 error
	addi $1,  $1, 1 	# number overflow
	# correct
	addi $sp, $sp, -3   # $sp = 0
	addi $1, $0, 0x7FFF # $1 = 0x00007FFF
	addi $1, $1, 1 		# $1 = 0x00008000
	lw   $2,  8($sp)    # $2 = 0xFFFF8000
	addi $2, $2, 1 		# $2 = 0xffff8001
	addi $2, $2, -2 	# $2 = 0xffff7fff
	# memory access
 	addi $1, $0, 0x400 
	addi $2, $0, -0x2BCD 
	sb $2, -1($1)		# store to 1023
	lw $4, 1020($0) 	# $4 = 0x00000033
	sh $2, -4($1)		# store to 1020
	lw $5, 1020($0)		# $5 = 0xD4330033
	sb $2, -2($1)		# store to 1022
	lw $3, 1020($0)		# $03 = 0xD4333333
	sh $2, -8($1)		# store to 1016
	lw $7, 1016($0)		# $07 = 0xD4330000		
	sw $2, -8($1)		# store to 1016
	lw $6, 1016($0)		# $06 = 0xFFFFD433
	sb $2, -10($1)		# store to 1014
	lw $9, -12($1) 		# $9 = 0x00003300
	sh $2, 1014($0)		# store to 1014
	lw $8, -12($1) 		# $8 = 0x0000D433
	# lb & lbu
	lbu $14, 1023($0)	# $14 = 0x00000033 
	addi $14, $14, 0x80 # $14 = 0x000000B3
	lb $15, 1023($0)	# $15 = 0xFFFFFFB3
	# addi & bne & sll
	addi $10, $10, -0x0fff # $10 = 0xFFFFF001
	addi $11, $0, -0x7ffe # $11 = 0xFFFF8002
Back:	
	and $10, $10, $11 	# 
	sll $11, $11, 2 	# 
	bne $10, $0, Back 	# Back = -3
	# nor & bne & sra
	addi $10, $10, 7	# 
	lui $11, -0x0fff 	# 
Try:
	nor $12, $10, $11 	# 
	sra $11, $11, 1 	#
	bne $12, $0, Try 	# Try = -3
	# lb, odd offset 
	lb $0, 13($0)   	# write to $0 (0d00 0080)
	lb $1, 29($0)   	# Ok
	# number overflow
	lw $1, 0($0) 		#3 $1 = 0x80000000      data at 0
	lw $20, 0($0) 		#4 $20 = 0x80000000      data at 0
	add $0,$20,$1       # write 0 & overflow
	add $20,$20,$1      #5 number overflow
	sub $1, $0, $1 		#6 0+(-0x8000000) = 0x8000000 , no number overflow
	sub $25, $8, $1 	#7 D433+(-0x80000000) = 0x8000D433
	addi $12, $1, -1 	#8 0x80000000 - 1 = $12 0x7FFFFFFF, number overflow, 
	sub $12, $12, $1 	#9 0x7FFFFFFF +(-0x80000000)=$12 0xffffffff
	addi $1, $1, -1 	#10 0x80000000 +(-1) = $1 0x7FFFFFFF, number overflow,
	addi $1, $1, 1 		#11 0x7FFFFFFF+1 = $1 0x80000000, number overflow
	# ori
	ori $13, $6 , 0xef12 #12 $13 = 0xFFFFEF12
	# lui
	lui $0, -0x7fff 	#13 write to $0
	lui $1, -0x7fff 	#14 $1 = 0x80010000
	sub $1, $1, $1 		#15 0x80010000 - 0x80010000 = 0x00000000
	# multi error
	lw $1, 0($sp)
	sub $0, $1, $1 		# write to $0
	add $30, $30, $1 	# $30 => 0x80000000
	addi $5, $30, 0xffff #number overflow
	add $30, $0, $0     # $30 = 0
	addi $30, $0 ,1023
	# address overflow
	lh	$0,	0($30)      # two error - - Write $0 error & Address overflow
	# four errors
	lh	$0,	0xffff($30)	# Write $0 error & number overflow & Address overflow & Misalignment error
	halt