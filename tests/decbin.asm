.data 
num: .word 96
dec: .asciz "Decimal: "
bin: .asciz "\nInverted Binary: "
nel: .asciz "\n"

.text
	lw s0, num
	la a0, dec
	jal ra, sprint
	addi a0, s0, 0
	li a7,1
	ecall
	
	la a0, nel
	jal ra, sprint
	la a0, bin
	jal ra, sprint
	
	addi a1, s0, 0
loop:
	jal ra, mod_proc
	addi a0, a0, 1
	li a7, 1
	ecall
	srli a1, a1, 1
	bne a1, zero, loop
	
	la a0, nel
	jal ra, sprint
	
	li a7, 10
	ecall
	
mod_proc:	#a1: number, a0: number%2
	srli t6, a1, 1
	add t6, t6, t6
	sub a0, a1, t6
	addi a0, a0, -1
	ret
	
sprint:
	li a7, 4
	ecall
	ret
	
