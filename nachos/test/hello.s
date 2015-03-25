	.file	1 "hello.c"
gcc2_compiled.:
__gnu_compiled_c:
	.rdata
	.align	2
$LC0:
	.ascii	"** ** ** Bonjour le monde ** ** **\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,24
	sw	$31,16($sp)
	jal	__gccmain
	la	$4,$LC0
	jal	printf
$L2:
	lw	$31,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	main
