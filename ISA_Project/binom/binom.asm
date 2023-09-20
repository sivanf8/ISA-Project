# binom coeffient SIMP
# Initialize n,k
.word 256 6
.word 257 4

add $sp, $zero, $imm, 1		# set $sp = 1
sll $sp, $sp, $imm, 7		# set $sp = 1 << 7 = 128
lw $a0, $zero, $imm, 256      # get n from address 256
lw $a1, $zero, $imm, 257      # get k from address 257
jal $imm, $zero, $zero, binom	# calc $v0 = binom(n, k)
sw $v0, $zero, $imm, 258	# store binom(n, k) in 258
halt $zero, $zero, $zero, 0	# halt

Base:
	add $v0, $zero, $imm, 1	# in base case return $v0=1
	beq $imm, $zero, $zero, L2	# jump to L2


# binomial coefficient function
binom:
	add $sp, $sp, $imm, -4		# adjust stack for 4 items
	sw $s0, $sp, $imm, 3		# save $s0
	sw $ra, $sp, $imm, 2		# save return address
	sw $a0, $sp, $imm, 1		# save n
	sw $a1, $sp, $imm, 0		# save k
	lw $t0, $sp, $imm, 0		# $t0= k
	beq $imm, $t0, $zero, Base  	# jump to Base if k == 0
	lw $t1, $sp, $imm, 1		# $t1= n
	sub $t2, $t1, $t0, 0		# $t2 = n - k
	beq $imm, $t2, $zero, Base	# jump to Base if n == k
	bgt $imm, $a0, $a1, L1		# jump to L1 if n > k
L1:	
	sub $a0, $a0, $imm, 1		# calculate n - 1
	sub $a1, $a1, $imm, 1		# calculate k - 1
	jal $imm, $zero, $zero, binom	# calc $v0 = binom(n-1, k-1)
	add $s0, $v0, $imm, 0		# $s0 = binom(n-1, k-1)
	lw $a0, $sp, $imm, 1		# restore n
      sub $a0, $a0, $imm, 1		# calculate n - 1
	lw $a1, $sp, $imm, 0		# restore k
	jal $imm, $zero, $zero, binom	# calc $v0 = binom(n-1, k)
	add $v0, $v0, $s0, 0		# $v0 = binom(n-1, k-1) + binom(n-1, k)
	lw $a1, $sp, $imm, 0		# restore $a1
      lw $a0, $sp, $imm, 1		# restore $a0
	lw $ra, $sp, $imm, 2		# restore $ra
	lw $s0, $sp, $imm, 3		# restore $s0
	

	
L2:
	add $sp, $sp, $imm, 4		# pop 4 items from stack
	beq $ra, $zero, $zero, 0	# and return