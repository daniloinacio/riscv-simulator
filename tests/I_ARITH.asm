.data 
q1: .word 56

.text
	lw t0, q1
	
	addi t1, t0, -57
	lui  t3, 65535
	slti t4, t0, -1
	sltiu t5,t0, -1
	andi t6, t0, 49
	ori  s1, t0, 38
	xori s0, t0, 89
	li a7, 10
	ecall
