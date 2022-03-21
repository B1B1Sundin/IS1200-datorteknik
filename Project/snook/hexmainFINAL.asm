   # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Modified by Jonathan Hansson & Yohanna Sundin 2022-02-01
  # Copyright abandonded - this file is in the public domain.
  
  # 0 - 9 returns 0 - 9 and 10 - 15 returns A - F

	.text
main:
	li	$a0,15		# change this to test different values

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window
	
stop:	j	stop		# stop after one run
	nop			# delay slot filler (just in case)


hexasc:
	li	$t0, 0x00000009		
	andi	$v0, $a0, 0x0000000f 		# "masking" with 15, making the rest 0, takes input register $a0 results in $v0
	
	# branch if !($t0 is LESS or EQUAL than our input !($t0 < $v0). Our input is somewhere between 0 - 9. 
	# Otherwise continue, the integer is somewhere inbetween (10-15)
	ble 	$v0, $t0, number	
	
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

