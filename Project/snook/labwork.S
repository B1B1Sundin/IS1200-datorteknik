  # labwork.S
  # Written 2015-2017 by F Lundevall
  #Modified by Jonathan Hansson & Yohanna Sundin 2022-02-01
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.global delay
.global hexasc
.global number
.global time2string
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
  
 hexasc:
	li	$t0, 0x00000009		
	andi	$v0, $a0, 0x0000000f 		# "masking" with 15, making the rest 0, takes input register $a0 results in $v0
	
	# branch if !($t0 is LESS or EQUAL than our input !($t0 < $v0). Our input is somewhere between 0 - 9. 
	# Otherwise continue, the integer is somewhere inbetween (10-15)
	ble 	$v0, $t0, number
	nop	
	
	# the value is 10-15, add the decimal 55 or 0x37 to make the value to ascii (10-15)
	# ex. the value 11 + 55 = 66 or 0x42 which is the char B in ASCii
	addi	$v0, $v0, 0x00000037	
	jr 	$ra
	nop
	
# the hex value for 0 - 9 is in hex 0x30 - 0x39 or in decimal 48 - 57
number:
	addi	$v0, $v0, 0x00000030	# the value is somewhere between 0-9, add 3 to make the value to ascii (0-9)
	jr	$ra
	nop
	
delay: #void delay(int ms), wait a n of ms, specified by the parameter value				
	li 	$t0, 0 # int i	
	
while_loop:
	beq     $a0, $zero, while_is_done # branch if(when) t0 equal to a0, change to ble for negative numbers
	sub 	$a0, $a0, 1 # ms--
	j 	for
	nop
	
	
for: # for( i = 0; i < 4711; i = i + 1 )
	beq  	$t0, 5000, for_is_done 	# branch if(when) t0 is equal to X. The higher number, the more slower it executes.
	addi	$t0,$t0,1 # i++
	j	for # repeat until branch at bge to "for_is_done"
	nop
	
for_is_done:
	move	$t0, $zero # reset the value of t0 for the next iteration
	j	while_loop # return to while, when ms is less or equal then 0, whilwloop branch to while_is_done
	nop
	
while_is_done:
	jr $ra #return to the adress that called the function delay2
 	nop
 	
time2string: 
	PUSH $ra
	PUSH $s0
	PUSH $s1
	
	move $s0,$a0 # contains timestr, we will overwrite this with the ascii version of mytime.
	move $s1,$a1 # contains mytime 	59:57
	
	srl 	$t0, $a1,12	# shift right 12 bits, Xx:xx	
	andi	$t0, $t0, 0xf	# mask out remainder bits to left, should work without this function, hexasc already masks the first byte
	move 	$a0, $t0	# move the value to argument for hexasc		
	jal 	hexasc			
	nop
	sb	$v0,0($s0)	# store the byte function return($v0) from hexasc in $s0, overwrite the text with ascii symbols of mytime
	
	srl 	$t1, $a1,8	# shift right 8 bits, xX:xx	
	andi	$t1, $t1,0xf	
	move 	$a0, $t1		
	jal 	hexasc			
	nop
	sb	$v0,1($s0)		
	
	li 	$t2, 0x3a	
	sb	$t2,2($s0)	# prints out :	
			
	srl 	$t3, $a1,4 	# shift right 4 bits, xx:Xx	
	andi	$t3, $t3,0xf		
	move 	$a0, $t3		
	jal 	hexasc			
	nop	
	sb	$v0,3($s0)		
	
	# doesnt need to right shift, already in place, xx:xX
	move 	$a0, $a1		
	jal 	hexasc			
	nop
	sb	$v0,4($s0)		
	
	li 	$t4, 0x00 	# the null byte 		
	sb	$t4,5($s0)	# 	
	
	POP $s1 # restore registers 
	POP $s0 
	POP $ra
	jr	$ra
	nop

