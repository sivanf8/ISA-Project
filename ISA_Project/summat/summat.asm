# summat algorithm SIMP

# Initialize mat1
.word 256 5
.word 257 31
.word 258 46
.word 259 12
.word 260 7
.word 261 25
.word 262 17
.word 263 200
.word 264 -99
.word 265 0x000000C
.word 266 100
.word 267 22
.word 268 54
.word 269 125
.word 270 77
.word 271 -66


# Initialize mat2	
.word 272 4
.word 273 45
.word 274 26
.word 275 71
.word 276 80
.word 277 531
.word 278 -13
.word 279 201
.word 0x118 3
.word 281 99
.word 282 0xfffffffF
.word 283 63
.word 284 -22
.word 285 96
.word 286 64
.word 287 2

# Set variables

add $t0, $zero, $imm, 256    # $t0 = 256 (start of mat1)
add $t1, $zero, $imm, 271    # $t1 = 271 (end of mat1)
# Loop through mat1
Loop:
  lw $t2, $t0, $zero, 0       # $t2 = mat1[i]
  lw $t3, $t0, $imm, 16       # $t3 = mat2[i]
  add $t2, $t3, $t2, 0        # $t2 =  mat1[i]+mat2[i]
  sw  $t2,$t0,$imm,32         #mat3[i] =  mat1[i]+mat2[i]
  add $t0, $t0, $imm, 1       # start++    
  ble $imm ,$t0, $t1, Loop    # correspond to C code to :while(start<=end) , go to Loop

# Halt
halt $zero, $zero, $zero, 0

