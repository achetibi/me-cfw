#ifndef ____MALLOC_H____

#define ____MALLOC_H____

int  mallocinit();

void *sctrlKernelMalloc(size_t size);
int sctrlKernelFree(void *p);
int  mallocterminate();

typedef struct _MemMgmtSubBlock
{
  struct _MemMgmtSubBlock *next;
  int pos; //bit 1 = useflag, >> 1 = pos
  int nblocks; //bit 1 = delflag, bit 2 = sizelock, >> 2 = num of blocks
} MemMgmtSubBlock;

typedef struct _MemMgmtBlock
{
  struct _MemMgmtBlock *next;//0
  MemMgmtSubBlock subblocks[0x14];//4
} MemMgmtBlock;

typedef struct _PartitionInfo
{
  struct _PartitionInfo *next; //0
  int addr; //4
  int size; //8
  int attr; //c
  struct _MemMgmtBlock *head;//0x10
  int nBlocksUsed; //14
  int unk_18;
} PartitionInfo;


#endif

