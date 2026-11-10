	.text
	.global __unmapself
	.hidden __unmapself
	.type __unmapself,@function
__unmapself:
	BSTART.SYS FALL
	addiw	zero, 215,	->a7
	acrc 1
	BSTART.SYS FALL
	c.movr	zero,	->a0
	addiw	zero, 93,	->a7
	acrc 1
	C.BSTART	DIRECT, __unmapself
	C.BSTOP
