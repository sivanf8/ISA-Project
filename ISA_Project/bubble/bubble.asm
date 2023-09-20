# Bubble sort algorithm SIMP

# Initialize array
.word 256 16
.word 257 15
.word 258 14
.word 259 13
.word 0x104 17
.word 261 11
.word 262 0xFfffffff
.word 263 9
.word 264 0x0000000C
.word 265 7
.word 266 -2
.word 267 5
.word 268 4
.word 269 3
.word 270 2
.word 271 1

# Set variables

add $t0, $zero, $imm, 256    # $t0 = 256 (start)
add $t1, $zero, $imm, 271    # $t1 = 271 (end)


# Loop through the array
outerLoop:
  add $t3, $t0, $zero, 0      # $t3 = $t0 (int i=start)
innerLoop:
  lw $s0, $t3, $zero, 0       # $s0 = array[i]
  lw $s1, $t3, $imm, 1        # $s1 = array[i+1]
  ble $imm, $s0, $s1, Skip    # if (array[i]<=array[i+1]) skip swap
  sw $s0, $t3, $imm, 1        # swap 
  sw $s1, $t3, $zero, 0       # swap

Skip:
  add $t3, $t3, $imm, 1             # i++
  blt $imm ,$t3, $t1, innerLoop     # correspond to C code to: while(i<end) ,go to innerLoop
  sub $t1, $t1, $imm, 1             # end--
  ble $imm ,$t0, $t1, outerLoop     # correspond to C code to :while(start<=end) , go to outerLoop

# Halt
halt $zero, $zero, $zero, 0