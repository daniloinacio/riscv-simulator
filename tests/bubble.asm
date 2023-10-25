.data
arr:	.word 3, 4, 5, 2, 1, 9, 0
size:	.word 7

.text
inicio:
	la s3, arr		# endereco base arr
	#j inicio
	la s0, size		# i = size
	lw s0, 0(s0)
foroo:	
	blez s0, FIM		# if i <= 0 encerra
	mv s1, zero		# j = 0
forii:	
	bge  s1, s0, forout	# if j >= i encerra laco
	slli t0, s1, 2		# t0 = 4*j
	add  t0, t0, s3		# &arr[j]
	lw   t1, 0(t0)		# t1 = arr[j]
	lw   t2, 4(t0)		# t2 = arr[j+1]
	bge  t2, t1, forin	# if arint[j+1] >= arint[j]) pula
	sw   t1, 4(t0)
	sw   t2, 0(t0)
forin:	
	addi s1, s1, 1
	j forii
forout: 
	addi s0, s0, -1
	j foroo
FIM:
	or zero,zero,zero
	addi a7, zero, 10
	ecall 
