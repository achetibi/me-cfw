	.file	1 "dosfs.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	.globl	wcharcpy
	.set	nomips16
	.ent	wcharcpy
wcharcpy:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$3,$4
	j	$L2
	li	$7,-1			# 0xffffffffffffffff

$L3:
	addiu	$5,$5,2
	sb	$2,0($3)
	addiu	$3,$3,1
$L2:
	addiu	$6,$6,-1
	bnel	$6,$7,$L3
	lbu	$2,0($5)

	j	$31
	move	$2,$4

	.set	macro
	.set	reorder
	.end	wcharcpy
	.size	wcharcpy, .-wcharcpy
	.align	2
	.globl	DFS_GetPtnStart
	.set	nomips16
	.ent	DFS_GetPtnStart
DFS_GetPtnStart:
	.frame	$sp,24,$31		# vars= 0, regs= 6/0, args= 0, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$17,4($sp)
	andi	$17,$6,0x00ff
	sltu	$2,$17,4
	sw	$20,16($sp)
	sw	$19,12($sp)
	sw	$18,8($sp)
	sw	$16,0($sp)
	sw	$31,20($sp)
	move	$16,$5
	move	$18,$7
	move	$19,$8
	beq	$2,$0,$L6
	move	$20,$9

	move	$4,$5
	li	$6,1			# 0x1
	jal	DFS_HostReadSector
	move	$5,$0

	bne	$2,$0,$L6
	addiu	$2,$17,28

	sll	$2,$2,4
	addu	$2,$16,$2
	lbu	$3,7($2)
	lbu	$4,8($2)
	lbu	$5,9($2)
	lbu	$2,6($2)
	sll	$3,$3,8
	sll	$4,$4,16
	or	$3,$3,$4
	or	$3,$3,$2
	sll	$5,$5,24
	beq	$18,$0,$L7
	or	$6,$3,$5

	sll	$2,$17,4
	addu	$2,$16,$2
	lbu	$2,446($2)
	sb	$2,0($18)
$L7:
	beq	$19,$0,$L8
	sll	$2,$17,4

	addu	$2,$16,$2
	lbu	$2,450($2)
	sb	$2,0($19)
$L8:
	beq	$20,$0,$L9
	addiu	$2,$17,28

	sll	$2,$2,4
	addu	$2,$16,$2
	lbu	$3,11($2)
	lbu	$4,12($2)
	lbu	$5,13($2)
	lbu	$2,10($2)
	sll	$3,$3,8
	sll	$4,$4,16
	or	$3,$3,$4
	or	$3,$3,$2
	sll	$5,$5,24
	or	$3,$3,$5
	j	$L9
	sw	$3,0($20)

$L6:
	li	$6,-1			# 0xffffffffffffffff
$L9:
	lw	$31,20($sp)
	move	$2,$6
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	DFS_GetPtnStart
	.size	DFS_GetPtnStart, .-DFS_GetPtnStart
	.align	2
	.globl	DFS_GetVolInfo
	.set	nomips16
	.ent	DFS_GetVolInfo
DFS_GetVolInfo:
	.frame	$sp,16,$31		# vars= 0, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$17,4($sp)
	move	$17,$7
	sw	$18,8($sp)
	sw	$16,0($sp)
	sb	$4,0($7)
	sw	$6,16($17)
	move	$4,$5
	move	$18,$6
	move	$16,$5
	move	$5,$6
	sw	$31,12($sp)
	jal	DFS_HostReadSector
	li	$6,1			# 0x1

	bne	$2,$0,$L13
	li	$3,-1			# 0xffffffffffffffff

	lbu	$2,13($16)
	sb	$2,20($17)
	lbu	$2,15($16)
	lbu	$3,14($16)
	sll	$2,$2,8
	or	$2,$2,$3
	sh	$2,22($17)
	lbu	$2,20($16)
	lbu	$3,19($16)
	sll	$2,$2,8
	or	$2,$2,$3
	bne	$2,$0,$L14
	sw	$2,24($17)

	lbu	$2,33($16)
	lbu	$3,34($16)
	lbu	$5,32($16)
	lbu	$4,35($16)
	sll	$2,$2,8
	sll	$3,$3,16
	or	$2,$2,$3
	or	$2,$2,$5
	sll	$4,$4,24
	or	$2,$2,$4
	sw	$2,24($17)
$L14:
	lbu	$2,23($16)
	lbu	$3,22($16)
	addiu	$6,$17,2
	sll	$2,$2,8
	or	$2,$2,$3
	bne	$2,$0,$L15
	sw	$2,28($17)

	lbu	$2,37($16)
	lbu	$3,38($16)
	lbu	$5,36($16)
	lbu	$4,39($16)
	sll	$2,$2,8
	sll	$3,$3,16
	or	$2,$2,$3
	or	$2,$2,$5
	sll	$4,$4,24
	or	$2,$2,$4
	sw	$2,28($17)
	move	$4,$6
	j	$L22
	addiu	$5,$16,71

$L15:
	move	$4,$6
	addiu	$5,$16,43
$L22:
	jal	memcpy
	li	$6,11			# 0xb

	sb	$0,13($17)
	lbu	$2,18($16)
	lbu	$3,17($16)
	lhu	$4,22($17)
	sll	$2,$2,8
	or	$2,$2,$3
	sh	$2,32($17)
	addu	$5,$18,$4
	andi	$2,$2,0xffff
	sw	$5,40($17)
	beq	$2,$0,$L17
	lw	$3,28($17)

	sll	$2,$2,5
	sll	$3,$3,1
	addiu	$2,$2,511
	addu	$3,$5,$3
	sra	$2,$2,9
	addu	$2,$3,$2
	sw	$2,48($17)
	j	$L18
	sw	$3,44($17)

$L17:
	sll	$2,$3,1
	addu	$2,$5,$2
	sw	$2,48($17)
	lbu	$2,45($16)
	lbu	$3,46($16)
	lbu	$4,47($16)
	lbu	$5,44($16)
	sll	$2,$2,8
	sll	$3,$3,16
	or	$2,$2,$3
	or	$2,$2,$5
	sll	$4,$4,24
	or	$2,$2,$4
	sw	$2,44($17)
$L18:
	lw	$2,24($17)
	lw	$4,48($17)
	lbu	$3,20($17)
	subu	$2,$2,$4
	bne	$3,$0,1f
	divu	$0,$2,$3
	break	7
1:
	mflo	$3
	sltu	$2,$3,4085
	beq	$2,$0,$L19
	sw	$3,36($17)

	j	$L23
	sb	$0,1($17)

$L19:
	li	$2,65525			# 0xfff5
	sltu	$2,$3,$2
	beql	$2,$0,$L20
	li	$2,2

	li	$2,1
$L20:
	sb	$2,1($17)
$L23:
	move	$3,$0
$L13:
	lw	$31,12($sp)
	move	$2,$3
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	DFS_GetVolInfo
	.size	DFS_GetVolInfo, .-DFS_GetVolInfo
	.align	2
	.set	nomips16
	.ent	DFS_CanonicalToDir
DFS_CanonicalToDir:
	.frame	$sp,16,$31		# vars= 0, regs= 3/0, args= 0, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$17,8($sp)
	sw	$16,4($sp)
	move	$17,$4
	move	$16,$5
	li	$6,11			# 0xb
	li	$5,32			# 0x20
	sw	$31,12($sp)
	jal	memset
	addiu	$16,$16,1

	sb	$0,11($17)
	move	$4,$17
	li	$9,47			# 0x2f
	li	$8,46			# 0x2e
	j	$L26
	addiu	$7,$17,8

$L31:
	beq	$2,$0,$L27
	nop

	j	$L33
	sb	$5,0($4)

$L27:
	bnel	$3,$8,$L29
	sb	$3,0($4)

	j	$L28
	move	$4,$7

$L29:
$L33:
	addiu	$4,$4,1
$L28:
	addiu	$16,$16,1
$L26:
	lbu	$3,-1($16)
	subu	$2,$4,$17
	slt	$6,$2,11
	addiu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,26
	beq	$3,$0,$L30
	addiu	$5,$3,-32

	beq	$3,$9,$L34
	lw	$31,12($sp)

	bne	$6,$0,$L31
	nop

$L30:
	lw	$31,12($sp)
$L34:
	move	$2,$17
	lw	$17,8($sp)
	lw	$16,4($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	DFS_CanonicalToDir
	.size	DFS_CanonicalToDir, .-DFS_CanonicalToDir
	.align	2
	.globl	cache_set
	.set	nomips16
	.ent	cache_set
cache_set:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(cache_flag)
	beq	$4,$0,$L38
	sw	$4,%lo(cache_flag)($2)

	lui	$4,%hi(LFN_buff)
	addiu	$4,$4,%lo(LFN_buff)
	move	$5,$0
	j	memset
	li	$6,128			# 0x80

$L38:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	cache_set
	.size	cache_set, .-cache_set
	.align	2
	.globl	cache_LFN
	.set	nomips16
	.ent	cache_LFN
cache_LFN:
	.frame	$sp,16,$31		# vars= 0, regs= 3/0, args= 0, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(cache_flag)
	lw	$2,%lo(cache_flag)($2)
	addiu	$sp,$sp,-16
	sw	$16,4($sp)
	sw	$31,12($sp)
	move	$16,$4
	sw	$17,8($sp)
	beq	$2,$0,$L41
	li	$4,-1			# 0xffffffffffffffff

	lbu	$17,0($16)
	andi	$2,$17,0x40
	beq	$2,$0,$L48
	andi	$2,$17,0xf

	jal	cache_set
	li	$4,1			# 0x1

	andi	$2,$17,0xf
$L48:
	addiu	$2,$2,-1
	li	$3,13			# 0xd
	mult	$2,$3
	lui	$2,%hi(LFN_buff)
	addiu	$2,$2,%lo(LFN_buff)
	addiu	$5,$16,1
	addiu	$6,$16,11
	mflo	$3
	j	$L43
	addu	$4,$2,$3

$L44:
	addiu	$5,$5,2
	sb	$2,0($4)
	addiu	$4,$4,1
$L43:
	bnel	$5,$6,$L44
	lbu	$2,0($5)

	lui	$2,%hi(LFN_buff)
	addiu	$3,$3,5
	addiu	$2,$2,%lo(LFN_buff)
	addu	$5,$2,$3
	addiu	$4,$16,14
	j	$L45
	addiu	$6,$16,26

$L46:
	addiu	$4,$4,2
	sb	$2,0($5)
	addiu	$5,$5,1
$L45:
	bnel	$4,$6,$L46
	lbu	$2,0($4)

	lbu	$4,28($16)
	lui	$2,%hi(LFN_buff)
	addiu	$3,$3,6
	addiu	$2,$2,%lo(LFN_buff)
	addu	$2,$2,$3
	sb	$4,0($2)
	lbu	$3,30($16)
	move	$4,$0
	sb	$3,1($2)
$L41:
	lw	$31,12($sp)
	move	$2,$4
	lw	$17,8($sp)
	lw	$16,4($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	cache_LFN
	.size	cache_LFN, .-cache_LFN
	.align	2
	.globl	dircacahe
	.set	nomips16
	.ent	dircacahe
dircacahe:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	move	$2,$4
	lui	$4,%hi(dircache_name)
	sw	$16,0($sp)
	addiu	$4,$4,%lo(dircache_name)
	move	$16,$5
	li	$6,64			# 0x40
	sw	$31,4($sp)
	jal	strncpy
	move	$5,$2

	lui	$4,%hi(dircache_info)
	move	$5,$16
	addiu	$4,$4,%lo(dircache_info)
	jal	memcpy
	li	$6,16			# 0x10

	lw	$5,8($16)
	lui	$4,%hi(sector_cache)
	lw	$31,4($sp)
	lw	$16,0($sp)
	addiu	$4,$4,%lo(sector_cache)
	li	$6,512			# 0x200
	j	memcpy
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	dircacahe
	.size	dircacahe, .-dircacahe
	.align	2
	.globl	dircache_search
	.set	nomips16
	.ent	dircache_search
dircache_search:
	.frame	$sp,16,$31		# vars= 0, regs= 3/0, args= 0, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	move	$2,$4
	lui	$4,%hi(dircache_name)
	sw	$17,8($sp)
	sw	$31,12($sp)
	sw	$16,4($sp)
	move	$17,$5
	addiu	$4,$4,%lo(dircache_name)
	jal	strcmp
	move	$5,$2

	bne	$2,$0,$L53
	li	$3,-1			# 0xffffffffffffffff

	lw	$16,8($17)
	lui	$5,%hi(sector_cache)
	addiu	$5,$5,%lo(sector_cache)
	move	$4,$16
	jal	memcpy
	li	$6,512			# 0x200

	lui	$5,%hi(dircache_info)
	addiu	$5,$5,%lo(dircache_info)
	move	$4,$17
	jal	memcpy
	li	$6,16			# 0x10

	sw	$16,8($17)
	move	$3,$0
$L53:
	lw	$31,12($sp)
	move	$2,$3
	lw	$17,8($sp)
	lw	$16,4($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	dircache_search
	.size	dircache_search, .-dircache_search
	.align	2
	.globl	DFS_GetFAT
	.set	nomips16
	.ent	DFS_GetFAT
DFS_GetFAT:
	.frame	$sp,56,$31		# vars= 24, regs= 7/0, args= 0, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$21,48($sp)
	sw	$20,44($sp)
	sw	$19,40($sp)
	sw	$17,32($sp)
	sw	$31,52($sp)
	sw	$18,36($sp)
	sw	$16,28($sp)
	lbu	$3,1($4)
	move	$20,$4
	move	$17,$5
	move	$19,$6
	bne	$3,$0,$L56
	move	$21,$7

	srl	$2,$7,1
	j	$L57
	addu	$16,$2,$7

$L56:
	li	$2,1			# 0x1
	beq	$3,$2,$L57
	sll	$16,$7,1

	li	$2,2			# 0x2
	bne	$3,$2,$L71
	li	$2,268369920			# 0xfff0000

	sll	$16,$7,2
$L57:
	move	$4,$16
	jal	ldiv
	li	$5,512			# 0x200

	lw	$4,40($20)
	lw	$6,0($19)
	sw	$3,4($sp)
	addu	$18,$2,$4
	sw	$2,16($sp)
	sw	$3,20($sp)
	beq	$18,$6,$L60
	sw	$2,0($sp)

	move	$4,$17
	move	$5,$18
	jal	DFS_HostReadSector
	li	$6,1			# 0x1

	bnel	$2,$0,$L59
	sw	$0,0($19)

$L61:
	sw	$18,0($19)
$L60:
	move	$4,$16
	jal	ldiv
	li	$5,512			# 0x200

	lbu	$4,1($20)
	sw	$2,8($sp)
	sw	$2,16($sp)
	sw	$3,20($sp)
	bne	$4,$0,$L63
	sw	$3,12($sp)

	li	$2,511			# 0x1ff
	bne	$3,$2,$L64
	addu	$2,$17,$3

	addiu	$16,$18,1
	move	$4,$17
	move	$5,$16
	li	$6,1			# 0x1
	jal	DFS_HostReadSector
	lbu	$18,511($17)

	beql	$2,$0,$L65
	sw	$16,0($19)

	j	$L59
	sw	$0,0($19)

$L65:
	lbu	$2,0($17)
	sll	$2,$2,8
	j	$L66
	or	$4,$2,$18

$L64:
	lbu	$3,1($2)
	lbu	$2,0($2)
	sll	$3,$3,8
	or	$4,$3,$2
$L66:
	andi	$2,$4,0xfff
	andi	$3,$21,0x1
	srl	$4,$4,4
	j	$L62
	movz	$4,$2,$3

$L63:
	li	$2,1			# 0x1
	bne	$4,$2,$L68
	li	$2,2			# 0x2

	addu	$2,$17,$3
	lbu	$3,1($2)
	lbu	$2,0($2)
	sll	$3,$3,8
	j	$L62
	or	$4,$3,$2

$L68:
	bne	$4,$2,$L71
	li	$2,268369920			# 0xfff0000

	addu	$2,$17,$3
	lbu	$3,1($2)
	lbu	$4,2($2)
	lbu	$5,3($2)
	lbu	$2,0($2)
	sll	$4,$4,16
	sll	$3,$3,8
	or	$3,$3,$4
	or	$3,$3,$2
	sll	$5,$5,24
	li	$2,268369920			# 0xfff0000
	or	$3,$3,$5
	ori	$2,$2,0xffff
	j	$L62
	and	$4,$3,$2

$L59:
	li	$2,268369920			# 0xfff0000
$L71:
	ori	$4,$2,0xfff7
$L62:
	lw	$31,52($sp)
	move	$2,$4
	lw	$21,48($sp)
	lw	$20,44($sp)
	lw	$19,40($sp)
	lw	$18,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	DFS_GetFAT
	.size	DFS_GetFAT, .-DFS_GetFAT
	.align	2
	.globl	DFS_ReadFile
	.set	nomips16
	.ent	DFS_ReadFile
DFS_ReadFile:
	.frame	$sp,104,$31		# vars= 64, regs= 10/0, args= 0, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lw	$2,24($4)
	lw	$3,16($4)
	addiu	$sp,$sp,-104
	sw	$fp,96($sp)
	subu	$3,$3,$2
	sltu	$2,$3,$8
	movz	$3,$8,$2
	sw	$23,92($sp)
	sw	$22,88($sp)
	sw	$21,84($sp)
	sw	$20,80($sp)
	sw	$19,76($sp)
	sw	$18,72($sp)
	sw	$17,68($sp)
	sw	$31,100($sp)
	sw	$16,64($sp)
	move	$18,$4
	move	$22,$7
	move	$21,$5
	move	$20,$6
	sw	$0,0($7)
	move	$19,$3
	move	$17,$0
	li	$fp,2			# 0x2
	j	$L87
	li	$23,65528			# 0xfff8

$L83:
	lw	$16,20($18)
	lw	$4,24($18)
	lbu	$5,20($2)
	addiu	$16,$16,-2
	lw	$17,48($2)
	mult	$5,$16
	sll	$5,$5,9
	jal	div
	mflo	$16

	move	$4,$3
	li	$5,512			# 0x200
	sw	$2,4($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	jal	div
	sw	$3,8($sp)

	lw	$4,24($18)
	li	$5,512			# 0x200
	addu	$17,$17,$2
	sw	$3,16($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	jal	div
	sw	$2,12($sp)

	addu	$17,$17,$16
	sw	$2,20($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	beq	$3,$0,$L74
	sw	$3,24($sp)

	move	$5,$17
	li	$6,1			# 0x1
	jal	DFS_HostReadSector
	move	$4,$21

	lw	$4,24($18)
	li	$5,512			# 0x200
	jal	div
	move	$17,$2

	li	$6,512			# 0x200
	subu	$4,$6,$3
	andi	$16,$4,0xffff
	sltu	$4,$19,$16
	sw	$2,28($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	sw	$3,32($sp)
	bne	$4,$0,$L75
	subu	$6,$6,$16

	addu	$5,$21,$6
	move	$4,$20
	jal	memcpy
	move	$6,$16

	lw	$2,24($18)
	subu	$19,$19,$16
	sw	$16,0($sp)
	addu	$2,$2,$16
	sw	$2,24($18)
	j	$L76
	addu	$20,$20,$16

$L75:
	addu	$5,$21,$6
	j	$L88
	move	$4,$20

$L74:
	sltu	$2,$19,512
	bne	$2,$0,$L77
	move	$5,$17

	move	$4,$20
	jal	DFS_HostReadSector
	li	$6,1			# 0x1

	move	$17,$2
	lw	$2,24($18)
	li	$3,512			# 0x200
	sw	$3,0($sp)
	addiu	$2,$2,512
	sw	$2,24($18)
	addiu	$19,$19,-512
	j	$L76
	addiu	$20,$20,512

$L77:
	move	$4,$21
	jal	DFS_HostReadSector
	li	$6,1			# 0x1

	move	$17,$2
	move	$4,$20
	move	$5,$21
$L88:
	jal	memcpy
	move	$6,$19

	lw	$2,24($18)
	addu	$20,$20,$19
	sw	$19,0($sp)
	addu	$2,$2,$19
	sw	$2,24($18)
	move	$19,$0
$L76:
	lw	$6,0($sp)
	lw	$2,0($22)
	lw	$3,0($18)
	addu	$2,$2,$6
	sw	$2,0($22)
	lbu	$5,20($3)
	lw	$4,24($18)
	sll	$5,$5,9
	jal	div
	subu	$4,$4,$6

	lw	$4,0($18)
	sw	$3,40($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	sw	$2,36($sp)
	lbu	$5,20($4)
	lw	$4,24($18)
	move	$16,$2
	jal	div
	sll	$5,$5,9

	sw	$3,48($sp)
	sw	$2,56($sp)
	sw	$3,60($sp)
	beq	$16,$2,$L87
	sw	$2,44($sp)

	lw	$4,0($18)
	sw	$0,0($sp)
	lbu	$3,1($4)
	bnel	$3,$0,$L78
	li	$2,1			# 0x1

	lw	$2,20($18)
	j	$L90
	sltu	$2,$2,4088

$L78:
	bne	$3,$2,$L81
	nop

	lw	$2,20($18)
	j	$L90
	sltu	$2,$2,$23

$L81:
	bne	$3,$fp,$L80
	li	$3,268369920			# 0xfff0000

	lw	$2,20($18)
	ori	$3,$3,0xfff8
	sltu	$2,$2,$3
$L90:
	beql	$2,$0,$L79
	li	$17,1			# 0x1

$L80:
	lw	$7,20($18)
	move	$5,$21
	jal	DFS_GetFAT
	move	$6,$sp

	sw	$2,20($18)
$L79:
$L87:
	beq	$19,$0,$L92
	lw	$31,100($sp)

	beql	$17,$0,$L83
	lw	$2,0($18)

	lw	$31,100($sp)
$L92:
	move	$2,$17
	lw	$fp,96($sp)
	lw	$23,92($sp)
	lw	$22,88($sp)
	lw	$21,84($sp)
	lw	$20,80($sp)
	lw	$19,76($sp)
	lw	$18,72($sp)
	lw	$17,68($sp)
	lw	$16,64($sp)
	j	$31
	addiu	$sp,$sp,104

	.set	macro
	.set	reorder
	.end	DFS_ReadFile
	.size	DFS_ReadFile, .-DFS_ReadFile
	.align	2
	.globl	DFS_GetNext
	.set	nomips16
	.ent	DFS_GetNext
DFS_GetNext:
	.frame	$sp,24,$31		# vars= 8, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$18,16($sp)
	sw	$17,12($sp)
	sw	$16,8($sp)
	sw	$31,20($sp)
	lbu	$2,5($5)
	move	$16,$5
	move	$17,$4
	sltu	$2,$2,16
	bne	$2,$0,$L94
	move	$18,$6

	lbu	$2,4($5)
	lw	$7,0($5)
	sb	$0,5($5)
	addiu	$2,$2,1
	andi	$8,$2,0x00ff
	bne	$7,$0,$L95
	sb	$8,4($5)

	lhu	$3,32($4)
	sll	$2,$8,4
	sltu	$2,$2,$3
	beql	$2,$0,$L96
	li	$5,1			# 0x1

	lw	$5,44($4)
	li	$6,1			# 0x1
	lw	$4,8($16)
	j	$L107
	addu	$5,$8,$5

$L95:
	lbu	$2,20($4)
	sltu	$2,$8,$2
	bne	$2,$0,$L98
	sltu	$2,$7,4087

	bne	$2,$0,$L99
	sb	$0,4($5)

	lbu	$3,1($4)
	beql	$3,$0,$L109
	lbu	$2,12($16)

	li	$2,65527			# 0xfff7
	sltu	$2,$7,$2
	bne	$2,$0,$L99
	li	$2,1			# 0x1

	beq	$3,$2,$L100
	li	$2,268369920			# 0xfff0000

	ori	$2,$2,0xfff7
	sltu	$2,$7,$2
	bne	$2,$0,$L99
	li	$2,2			# 0x2

	bnel	$3,$2,$L110
	lw	$5,8($16)

$L100:
	lbu	$2,12($16)
$L109:
	andi	$2,$2,0x1
	bne	$2,$0,$L101
	li	$5,5			# 0x5

	j	$L101
	li	$5,1			# 0x1

$L99:
	lw	$5,8($16)
$L110:
	move	$4,$17
	jal	DFS_GetFAT
	move	$6,$sp

	sw	$2,0($16)
$L98:
	lw	$2,0($16)
	lbu	$3,20($17)
	lbu	$5,4($16)
	addiu	$2,$2,-2
	mult	$3,$2
	lw	$2,48($17)
	lw	$4,8($16)
	li	$6,1			# 0x1
	addu	$5,$5,$2
	mflo	$3
	addu	$5,$5,$3
$L107:
	jal	DFS_HostReadSector
	nop

	bne	$2,$0,$L101
	li	$5,-1			# 0xffffffffffffffff

$L94:
	lbu	$5,5($16)
	lw	$2,8($16)
	move	$4,$18
	sll	$5,$5,5
	addu	$5,$2,$5
	jal	memcpy
	li	$6,32			# 0x20

	lbu	$4,0($18)
	bne	$4,$0,$L102
	li	$2,229			# 0xe5

	lbu	$2,12($16)
	xori	$2,$2,0x1
	j	$L101
	andi	$5,$2,0x1

$L102:
	beql	$4,$2,$L104
	sb	$0,0($18)

$L103:
	lbu	$2,11($18)
	li	$3,15			# 0xf
	andi	$2,$2,0xf
	bne	$2,$3,$L105
	li	$2,5			# 0x5

	jal	cache_LFN
	move	$4,$18

	j	$L104
	sb	$0,0($18)

$L105:
	bnel	$4,$2,$L111
	lbu	$2,5($16)

	li	$2,-27
	sb	$2,0($18)
$L104:
	lbu	$2,5($16)
$L111:
	move	$5,$0
	addiu	$2,$2,1
	sb	$2,5($16)
$L96:
$L101:
	lw	$31,20($sp)
	move	$2,$5
	lw	$18,16($sp)
	lw	$17,12($sp)
	lw	$16,8($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	DFS_GetNext
	.size	DFS_GetNext, .-DFS_GetNext
	.align	2
	.globl	DFS_OpenDir
	.set	nomips16
	.ent	DFS_OpenDir
DFS_OpenDir:
	.frame	$sp,80,$31		# vars= 48, regs= 7/0, args= 0, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-80
	sw	$18,60($sp)
	sw	$17,56($sp)
	sw	$16,52($sp)
	sw	$31,76($sp)
	sw	$21,72($sp)
	sw	$20,68($sp)
	sw	$19,64($sp)
	sb	$0,12($6)
	lb	$2,0($5)
	move	$16,$6
	move	$18,$5
	beq	$2,$0,$L113
	move	$17,$4

	jal	strlen
	move	$4,$5

	li	$3,1			# 0x1
	bnel	$2,$3,$L142
	lbu	$3,1($17)

	lbu	$3,0($18)
	li	$2,47			# 0x2f
	bnel	$3,$2,$L114
	lbu	$3,1($17)

$L113:
	lbu	$3,1($17)
	li	$2,2			# 0x2
	bne	$3,$2,$L115
	lw	$4,8($16)

	lw	$2,44($17)
	sb	$0,5($16)
	sb	$0,4($16)
	sw	$2,0($16)
	lw	$2,44($17)
	lbu	$5,20($17)
	lw	$3,48($17)
	addiu	$2,$2,-2
	mult	$5,$2
	li	$6,1			# 0x1
	mflo	$5
	j	$L138
	addu	$5,$5,$3

$L115:
	sb	$0,5($16)
	sw	$0,0($16)
	sb	$0,4($16)
	lw	$5,44($17)
	li	$6,1			# 0x1
$L138:
	jal	DFS_HostReadSector
	nop

	j	$L116
	move	$3,$2

$L114:
$L142:
	li	$2,2			# 0x2
	bne	$3,$2,$L117
	lw	$4,8($16)

	lw	$2,44($17)
	sb	$0,4($16)
	sb	$0,5($16)
	sw	$2,0($16)
	lw	$2,44($17)
	lbu	$5,20($17)
	lw	$3,48($17)
	addiu	$2,$2,-2
	mult	$5,$2
	li	$6,1			# 0x1
	mflo	$5
	j	$L137
	addu	$5,$5,$3

$L117:
	sw	$0,0($16)
	sb	$0,4($16)
	sb	$0,5($16)
	lw	$5,44($17)
	li	$6,1			# 0x1
$L137:
	jal	DFS_HostReadSector
	nop

	beq	$2,$0,$L120
	li	$3,47			# 0x2f

	j	$L116
	li	$3,-1			# 0xffffffffffffffff

$L121:
$L120:
	lbu	$2,0($18)
	beql	$2,$3,$L121
	addiu	$18,$18,1

	j	$L139
	addiu	$19,$sp,12

$L131:
	jal	DFS_CanonicalToDir
	move	$5,$18

	sb	$0,12($sp)
	move	$4,$17
$L143:
	move	$5,$16
	jal	DFS_GetNext
	move	$6,$19

	move	$4,$19
	move	$5,$sp
	bne	$2,$0,$L146
	li	$6,11			# 0xb

	jal	memcmp
	nop

	bne	$2,$0,$L143
	move	$4,$17

	move	$4,$19
	move	$5,$sp
$L146:
	jal	memcmp
	li	$6,11			# 0xb

	bne	$2,$0,$L134
	lbu	$2,23($sp)

	andi	$2,$2,0x10
	beq	$2,$0,$L126
	lbu	$3,39($sp)

	lbu	$2,1($17)
	bne	$2,$21,$L127
	lbu	$5,38($sp)

	lbu	$2,32($sp)
	lbu	$4,33($sp)
	sll	$3,$3,8
	sll	$2,$2,16
	or	$3,$3,$2
	or	$3,$3,$5
	sll	$4,$4,24
	or	$3,$3,$4
	j	$L128
	sw	$3,0($16)

$L127:
	sll	$2,$3,8
	or	$2,$2,$5
	sw	$2,0($16)
$L128:
	sb	$0,4($16)
	sb	$0,5($16)
	lw	$2,0($16)
	lbu	$5,20($17)
	lw	$3,48($17)
	addiu	$2,$2,-2
	mult	$5,$2
	lw	$4,8($16)
	li	$6,1			# 0x1
	mflo	$5
	jal	DFS_HostReadSector
	addu	$5,$5,$3

	beql	$2,$0,$L144
	lbu	$2,0($18)

	j	$L118
	li	$3,-1			# 0xffffffffffffffff

$L130:
$L134:
	lbu	$2,0($18)
$L144:
	beql	$2,$20,$L136
	addiu	$18,$18,1

	bnel	$2,$0,$L130
	addiu	$18,$18,1

	j	$L140
	nop

$L139:
	li	$21,2			# 0x2
	li	$20,47			# 0x2f
$L136:
	lbu	$2,0($18)
$L140:
	bne	$2,$0,$L131
	move	$4,$sp

	lw	$2,0($16)
	bne	$2,$0,$L116
	move	$3,$0

	j	$L116
	li	$3,3			# 0x3

$L118:
	j	$L141
	lw	$31,76($sp)

$L126:
	li	$3,3			# 0x3
$L116:
	lw	$31,76($sp)
$L141:
	move	$2,$3
	lw	$21,72($sp)
	lw	$20,68($sp)
	lw	$19,64($sp)
	lw	$18,60($sp)
	lw	$17,56($sp)
	lw	$16,52($sp)
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	DFS_OpenDir
	.size	DFS_OpenDir, .-DFS_OpenDir
	.align	2
	.globl	DFS_OpenFile
	.set	nomips16
	.ent	DFS_OpenFile
DFS_OpenFile:
	.frame	$sp,224,$31		# vars= 192, regs= 8/0, args= 0, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-224
	sw	$20,208($sp)
	addiu	$20,$sp,60
	sw	$31,220($sp)
	sw	$22,216($sp)
	sw	$18,200($sp)
	sw	$17,196($sp)
	sw	$16,192($sp)
	sw	$21,212($sp)
	sw	$19,204($sp)
	move	$16,$5
	sw	$0,8($8)
	sw	$0,0($8)
	sb	$6,9($8)
	sw	$0,4($8)
	sw	$0,12($8)
	sw	$0,16($8)
	sw	$0,20($8)
	sw	$0,24($8)
	li	$6,64			# 0x40
	move	$18,$4
	move	$4,$20
	move	$17,$8
	jal	strncpy
	move	$22,$7

	sb	$0,123($sp)
	move	$4,$16
	jal	strcmp
	move	$5,$20

	beq	$2,$0,$L179
	li	$3,4			# 0x4

	j	$L180
	lw	$31,220($sp)

$L150:
	jal	strcpy
	nop

	j	$L181
	lbu	$2,60($sp)

$L179:
	li	$19,47			# 0x2f
	addiu	$16,$sp,61
	lbu	$2,60($sp)
$L181:
	move	$4,$20
	beq	$2,$19,$L150
	move	$5,$16

	addiu	$16,$sp,60
$L152:
	lbu	$2,0($16)
	beq	$2,$0,$L151
	addiu	$3,$16,1

	j	$L152
	move	$16,$3

$L151:
	addiu	$4,$sp,60
	li	$3,47			# 0x2f
$L177:
	sltu	$2,$4,$16
	beq	$2,$0,$L182
	lbu	$2,0($16)

	bnel	$2,$3,$L177
	addiu	$16,$16,-1

	lbu	$2,0($16)
$L182:
	li	$21,1			# 0x1
	xori	$2,$2,0x2f
	sltu	$2,$2,1
	addu	$16,$16,$2
	jal	strlen
	move	$4,$16

	slt	$2,$2,13
	beq	$2,$0,$L183
	move	$5,$16

	move	$4,$sp
	jal	DFS_CanonicalToDir
	move	$5,$16

	move	$21,$0
	move	$5,$16
$L183:
	jal	strcpy
	addiu	$4,$sp,124

	addiu	$4,$sp,60
	sltu	$2,$4,$16
	subu	$16,$16,$2
	lbu	$3,0($16)
	li	$2,47			# 0x2f
	beql	$3,$2,$L160
	sb	$0,0($16)

	bnel	$16,$4,$L184
	addiu	$16,$sp,12

	sb	$0,0($16)
$L160:
	addiu	$16,$sp,12
$L184:
	move	$4,$20
	move	$5,$16
	jal	dircache_search
	sw	$22,20($sp)

	beq	$2,$0,$L161
	move	$4,$18

	addiu	$5,$sp,60
	jal	DFS_OpenDir
	move	$6,$16

	bne	$2,$0,$L149
	li	$3,3			# 0x3

	move	$4,$20
	jal	dircacahe
	move	$5,$16

$L161:
	jal	cache_set
	li	$4,1			# 0x1

	lui	$2,%hi(LFN_buff)
	addiu	$22,$2,%lo(LFN_buff)
	addiu	$20,$sp,12
	addiu	$16,$sp,28
	j	$L175
	addiu	$19,$sp,124

$L171:
	beq	$2,$0,$L164
	move	$4,$22

	jal	strcmp
	move	$5,$19

	beq	$2,$0,$L185
	lbu	$2,39($sp)

$L164:
	bne	$21,$0,$L186
	move	$4,$18

	move	$4,$16
	move	$5,$sp
	jal	memcmp
	li	$6,11			# 0xb

	bne	$2,$0,$L186
	move	$4,$18

$L165:
	lbu	$2,39($sp)
$L185:
	andi	$2,$2,0x10
	bne	$2,$0,$L178
	nop

$L166:
	lw	$4,12($sp)
	sw	$18,0($17)
	sw	$0,24($17)
	bne	$4,$0,$L167
	lbu	$5,16($sp)

	lw	$2,44($18)
	addu	$2,$5,$2
	j	$L168
	sw	$2,4($17)

$L167:
	lbu	$2,20($18)
	addiu	$4,$4,-2
	lw	$3,48($18)
	mult	$2,$4
	addu	$3,$5,$3
	mflo	$2
	addu	$3,$3,$2
	sw	$3,4($17)
$L168:
	lbu	$2,17($sp)
	lbu	$5,55($sp)
	addiu	$2,$2,-1
	sb	$2,8($17)
	lbu	$3,1($18)
	li	$2,2			# 0x2
	bne	$3,$2,$L169
	lbu	$6,54($sp)

	lbu	$2,48($sp)
	lbu	$4,49($sp)
	sll	$3,$5,8
	sll	$2,$2,16
	or	$3,$3,$2
	or	$3,$3,$6
	sll	$4,$4,24
	or	$3,$3,$4
	j	$L170
	sw	$3,20($17)

$L169:
	sll	$2,$5,8
	or	$2,$2,$6
	sw	$2,20($17)
$L170:
	lbu	$2,57($sp)
	lbu	$3,58($sp)
	lbu	$5,56($sp)
	lbu	$4,59($sp)
	sll	$3,$3,16
	sll	$2,$2,8
	or	$2,$2,$3
	lw	$3,20($17)
	or	$2,$2,$5
	sll	$4,$4,24
	or	$2,$2,$4
	sw	$3,12($17)
	sw	$2,16($17)
	jal	cache_set
	move	$4,$0

	j	$L149
	move	$3,$0

$L175:
	move	$4,$18
$L186:
	move	$5,$20
	jal	DFS_GetNext
	move	$6,$16

	beq	$2,$0,$L171
	lbu	$2,28($sp)

$L178:
	jal	cache_set
	move	$4,$0

	li	$3,3			# 0x3
$L149:
	lw	$31,220($sp)
$L180:
	move	$2,$3
	lw	$22,216($sp)
	lw	$21,212($sp)
	lw	$20,208($sp)
	lw	$19,204($sp)
	lw	$18,200($sp)
	lw	$17,196($sp)
	lw	$16,192($sp)
	j	$31
	addiu	$sp,$sp,224

	.set	macro
	.set	reorder
	.end	DFS_OpenFile
	.size	DFS_OpenFile, .-DFS_OpenFile
	.globl	cache_flag
	.section	.bss,"aw",@nobits
	.align	2
	.type	cache_flag, @object
	.size	cache_flag, 4
cache_flag:
	.space	4

	.comm	LFN_buff,128,4

	.comm	dircache_name,64,4

	.comm	sector_cache,512,4

	.comm	dircache_info,16,4
	.ident	"GCC: (GNU) 4.3.5"
