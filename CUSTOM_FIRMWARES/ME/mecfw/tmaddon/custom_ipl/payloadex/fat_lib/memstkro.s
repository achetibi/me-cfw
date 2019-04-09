	.file	1 "memstkro.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	.globl	pspMsInit
	.set	nomips16
	.ent	pspMsInit
pspMsInit:
	.frame	$sp,8,$31		# vars= 8, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$4,-1139802112			# 0xffffffffbc100000
	ori	$3,$4,0x54
	lw	$2,0($3)
	ori	$5,$4,0x50
	ori	$6,$4,0x78
	ori	$2,$2,0x100
	sw	$2,0($3)
	lw	$2,0($5)
	ori	$4,$4,0x4c
	li	$3,-257			# 0xfffffffffffffeff
	ori	$2,$2,0x400
	sw	$2,0($5)
	lw	$2,0($6)
	li	$5,-1121976320			# 0xffffffffbd200000
	ori	$5,$5,0x3c
	ori	$2,$2,0x10
	sw	$2,0($6)
	lw	$2,0($4)
	addiu	$sp,$sp,-8
	move	$6,$5
	and	$2,$2,$3
	sw	$2,0($4)
	li	$2,32768			# 0x8000
	sw	$2,0($5)
$L2:
	lw	$2,0($6)
	andi	$2,$2,0x8000
	bne	$2,$0,$L2
	li	$5,-1121976320			# 0xffffffffbd200000

	ori	$3,$5,0x30
	li	$2,32772			# 0x8004
	sw	$2,0($3)
	li	$2,101711872			# 0x6100000
	ori	$4,$5,0x34
	ori	$2,$2,0x800
	sw	$2,0($4)
	ori	$3,$5,0x38
	sw	$0,0($4)
$L3:
	lw	$5,0($3)
	andi	$2,$5,0x1000
	beq	$2,$0,$L3
	li	$6,-1121976320			# 0xffffffffbd200000

	andi	$2,$5,0x300
	bne	$2,$0,$L25
	li	$2,-1121976320			# 0xffffffffbd200000

	j	$L24
	ori	$3,$6,0x30

$L21:
	lw	$2,0($3)
	andi	$5,$2,0x4000
	andi	$2,$2,0x100
	bne	$2,$0,$L25
	li	$2,-1121976320			# 0xffffffffbd200000

	beq	$5,$0,$L21
	nop

	lw	$2,0($7)
	sw	$2,0($4)
	addiu	$4,$4,4
	bne	$4,$6,$L21
	li	$2,-1121976320			# 0xffffffffbd200000

$L25:
	ori	$3,$2,0x38
$L13:
	lw	$2,0($3)
	andi	$2,$2,0x1000
	beq	$2,$0,$L13
	li	$2,-1121976320			# 0xffffffffbd200000

	ori	$6,$2,0x34
	ori	$8,$2,0x30
	li	$7,28673			# 0x7001
	ori	$5,$2,0x38
$L22:
	sw	$7,0($8)
$L8:
	lw	$2,0($5)
	andi	$3,$2,0x4000
	andi	$2,$2,0x100
	bne	$2,$0,$L22
	nop

	beq	$3,$0,$L8
	nop

	lw	$4,0($6)
	lw	$2,0($6)
$L9:
	lw	$2,0($5)
	andi	$3,$2,0x1000
	andi	$2,$2,0x100
	bne	$2,$0,$L22
	nop

	beq	$3,$0,$L9
	andi	$2,$4,0x80

	beq	$2,$0,$L22
	move	$2,$0

	j	$31
	addiu	$sp,$sp,8

$L24:
	li	$2,16392			# 0x4008
	sw	$2,0($3)
	ori	$7,$6,0x34
	ori	$3,$6,0x38
	move	$4,$sp
	j	$L21
	addiu	$6,$sp,8

	.set	macro
	.set	reorder
	.end	pspMsInit
	.size	pspMsInit, .-pspMsInit
	.align	2
	.globl	pspMsReadSector
	.set	nomips16
	.ent	pspMsReadSector
pspMsReadSector:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	sll	$2,$4,16
	li	$3,16711680			# 0xff0000
	and	$2,$2,$3
	ext	$6,$4,16,8
	or	$6,$6,$2
	li	$3,-16777216			# 0xffffffffff000000
	li	$2,65536			# 0x10000
	li	$7,-1121976320			# 0xffffffffbd200000
	and	$3,$4,$3
	ori	$2,$2,0x20
	andi	$4,$4,0xff00
	ori	$8,$7,0x34
	or	$3,$3,$2
	or	$6,$6,$4
	li	$2,36871			# 0x9007
	ori	$4,$7,0x30
	sw	$2,0($4)
	ori	$7,$7,0x38
	sw	$3,0($8)
	sw	$6,0($8)
$L27:
	lw	$3,0($7)
	andi	$2,$3,0x1000
	beq	$2,$0,$L27
	andi	$2,$3,0x300

	bne	$2,$0,$L28
	nop

	li	$2,-1121976320			# 0xffffffffbd200000
	ori	$3,$2,0x38
$L46:
	lw	$2,0($3)
	andi	$2,$2,0x2000
	beq	$2,$0,$L46
	li	$2,-1121976320			# 0xffffffffbd200000

	ori	$8,$2,0x34
	ori	$10,$2,0x30
	li	$9,28673			# 0x7001
	ori	$7,$2,0x38
$L45:
	sw	$9,0($10)
$L30:
	lw	$2,0($7)
	andi	$3,$2,0x4000
	andi	$2,$2,0x100
	bne	$2,$0,$L28
	nop

	beq	$3,$0,$L30
	nop

	lw	$4,0($8)
	lw	$2,0($8)
$L31:
	lw	$2,0($7)
	andi	$3,$2,0x1000
	andi	$2,$2,0x100
	bne	$2,$0,$L28
	li	$6,-1121976320			# 0xffffffffbd200000

	beq	$3,$0,$L31
	andi	$2,$4,0x20

	beq	$2,$0,$L45
	andi	$2,$4,0x40

	bne	$2,$0,$L28
	nop

	ori	$3,$6,0x30
	li	$2,8704			# 0x2200
	sw	$2,0($3)
	ori	$7,$6,0x34
	ori	$3,$6,0x38
	addiu	$6,$5,512
$L58:
	lw	$2,0($3)
	andi	$4,$2,0x4000
	andi	$2,$2,0x100
	bne	$2,$0,$L28
	nop

	beq	$4,$0,$L58
	nop

	lw	$2,0($7)
	sw	$2,0($5)
	addiu	$5,$5,4
	bne	$5,$6,$L58
	li	$2,-1121976320			# 0xffffffffbd200000

	ori	$4,$2,0x38
$L44:
	lw	$3,0($4)
	andi	$2,$3,0x1000
	beq	$2,$0,$L44
	andi	$2,$3,0x300

	bne	$2,$0,$L28
	nop

	li	$2,-1121976320			# 0xffffffffbd200000
	ori	$3,$2,0x38
$L43:
	lw	$2,0($3)
	andi	$2,$2,0x2000
	beq	$2,$0,$L43
	li	$2,-1121976320			# 0xffffffffbd200000

	ori	$6,$2,0x34
	ori	$8,$2,0x30
	li	$7,28673			# 0x7001
	ori	$5,$2,0x38
$L59:
	sw	$7,0($8)
$L37:
	lw	$2,0($5)
	andi	$3,$2,0x4000
	andi	$2,$2,0x100
	bne	$2,$0,$L59
	nop

	beq	$3,$0,$L37
	nop

	lw	$4,0($6)
	lw	$2,0($6)
$L38:
	lw	$2,0($5)
	andi	$3,$2,0x1000
	andi	$2,$2,0x100
	bne	$2,$0,$L59
	nop

	beq	$3,$0,$L38
	andi	$2,$4,0x80

	beq	$2,$0,$L59
	move	$2,$0

	j	$31
	nop

$L28:
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	pspMsReadSector
	.size	pspMsReadSector, .-pspMsReadSector
	.align	2
	.globl	pspMsWriteSector
	.set	nomips16
	.ent	pspMsWriteSector
pspMsWriteSector:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	pspMsWriteSector
	.size	pspMsWriteSector, .-pspMsWriteSector
	.ident	"GCC: (GNU) 4.3.5"
