    .ORG 0x0 # $sp = 0x03
	ori $8, $8, 0xd433  #1
	lw $6,  5($sp)      #2 $6 = 0xffffd433
	# number overflow
	lw $1, 0($0) 		#3 $1 = 0x80000000      data at 0
	lw $20, 0($0) 		#4 $20 = 0x80000000      data at 0
	add $20,$20,$1      #5 number overflow
	sub $1, $0, $1 		#6 0+(-0x8000000) = 0x8000000 , no number overflow
	sub $25, $8, $1 	#7 D433+(-0x80000000) = 0x8000D433
	addi $12, $1, -1 	#8 0x80000000 - 1 = $12 0x7FFFFFFF, number overflow, 
	sub $12, $12, $1 	#9 0x7FFFFFFF +(-0x80000000)=$12 0xffffffff
	addi $1, $1, -1 	#10 0x80000000 +(-0x80000000) = $1 0x7FFFFFFF, number overflow,
	addi $1, $1, 1 		#11 0x7FFFFFFF+1 = $1 0x80000000, number overflow
	# ori
	ori $13, $6 , 0xef12 #12 $13 = 0xFFFFEF12
	# lui
	lui $0, -0x7fff 	#13 write to $0
	lui $1, -0x7fff 	#14 $1 = 0x80010000
	sub $1, $1, $1 		#15 0x80010000 - 0x80010000 = 0x00000000
	# multi error
	sub $0, $1, $1 		#16 write to $0
	halt
	halt