	.file	1 "hostemu.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.text
	.align	2
	.globl	DFS_HostWriteSector
	.set	nomips16
	.ent	DFS_HostWriteSector
DFS_HostWriteSector:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	DFS_HostWriteSector
	.size	DFS_HostWriteSector, .-DFS_HostWriteSector
	.align	2
	.globl	close_ms_file
	.set	nomips16
	.ent	close_ms_file
close_ms_file:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	close_ms_file
	.size	close_ms_file, .-close_ms_file
	.align	2
	.globl	read_ms_file
	.set	nomips16
	.ent	read_ms_file
read_ms_file:
	.frame	$sp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	move	$6,$4
	move	$8,$5
	lui	$4,%hi(fi)
	lui	$5,%hi(sector)
	move	$7,$sp
	addiu	$4,$4,%lo(fi)
	addiu	$5,$5,%lo(sector)
	sw	$31,12($sp)
	jal	DFS_ReadFile
	sw	$0,0($sp)

	lw	$31,12($sp)
	lw	$2,0($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	read_ms_file
	.size	read_ms_file, .-read_ms_file
	.align	2
	.globl	open_ms_file
	.set	nomips16
	.ent	open_ms_file
open_ms_file:
	.frame	$sp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$5,$4
	lui	$7,%hi(sector)
	lui	$4,%hi(vi)
	lui	$8,%hi(fi)
	addiu	$sp,$sp,-8
	addiu	$4,$4,%lo(vi)
	addiu	$7,$7,%lo(sector)
	addiu	$8,$8,%lo(fi)
	sw	$31,4($sp)
	jal	DFS_OpenFile
	li	$6,1			# 0x1

	lw	$31,4($sp)
	li	$3,-1			# 0xffffffffffffffff
	movz	$3,$0,$2
	move	$2,$3
	j	$31
	addiu	$sp,$sp,8

	.set	macro
	.set	reorder
	.end	open_ms_file
	.size	open_ms_file, .-open_ms_file
	.align	2
	.globl	init_ms
	.set	nomips16
	.ent	init_ms
init_ms:
	.frame	$sp,16,$31		# vars= 8, regs= 2/0, args= 0, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$16,8($sp)
	sw	$31,12($sp)
	jal	pspMsInit
	lui	$16,%hi(sector)

	move	$6,$0
	move	$4,$0
	addiu	$5,$16,%lo(sector)
	move	$7,$sp
	addiu	$8,$sp,1
	jal	DFS_GetPtnStart
	addiu	$9,$sp,4

	move	$6,$2
	li	$2,-1			# 0xffffffffffffffff
	beq	$6,$2,$L13
	li	$3,-1			# 0xffffffffffffffff

	lui	$7,%hi(vi)
	addiu	$5,$16,%lo(sector)
	addiu	$7,$7,%lo(vi)
	jal	DFS_GetVolInfo
	move	$4,$0

	beq	$2,$0,$L13
	move	$3,$0

	li	$3,-1			# 0xffffffffffffffff
$L13:
	lw	$31,12($sp)
	move	$2,$3
	lw	$16,8($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	init_ms
	.size	init_ms, .-init_ms
	.align	2
	.globl	DFS_HostReadSector
	.set	nomips16
	.ent	DFS_HostReadSector
DFS_HostReadSector:
	.frame	$sp,16,$31		# vars= 0, regs= 4/0, args= 0, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-16
	sw	$18,8($sp)
	sw	$17,4($sp)
	sw	$16,0($sp)
	sw	$31,12($sp)
	move	$18,$5
	move	$17,$6
	j	$L16
	move	$16,$4

$L19:
	jal	pspMsReadSector
	addiu	$17,$17,-1

	bgezl	$2,$L21
	move	$4,$18

	j	$L18
	li	$2,-1			# 0xffffffffffffffff

$L16:
	move	$4,$18
$L21:
	move	$5,$16
	addiu	$18,$18,1
	bne	$17,$0,$L19
	addiu	$16,$16,512

	move	$2,$0
$L18:
	lw	$31,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	j	$31
	addiu	$sp,$sp,16

	.set	macro
	.set	reorder
	.end	DFS_HostReadSector
	.size	DFS_HostReadSector, .-DFS_HostReadSector
	.local	sector
	.comm	sector,512,4
	.local	vi
	.comm	vi,52,4
	.local	fi
	.comm	fi,28,4
	.ident	"GCC: (GNU) 4.3.5"
