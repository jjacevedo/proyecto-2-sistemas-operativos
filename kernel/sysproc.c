#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

struct sysinfo {
  uint64 freemem;
  uint64 usedpages;
  uint64 availpages;
  int    nrunnable;
};

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0)
      return -1;
  } else {
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;
  argint(0, &pid);
  return kkill(pid);
}

uint64
sys_uptime(void)
{
  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  char name[64];
  if (argstr(0, name, sizeof(name)) < 0)
    return -1;
  extern char traced_syscall[64];
  safestrcpy(traced_syscall, name, sizeof(traced_syscall));
  return 0;
}

uint64
sys_sysinfo(void)
{
  uint64 addr;
  argaddr(0, &addr);

  uint64 free = freepages();
  uint64 total = (PHYSTOP - KERNBASE) / PGSIZE;

  struct sysinfo info;
  info.freemem    = (free * PGSIZE) / (1024 * 1024);
  info.availpages = free;
  info.usedpages  = total - free;
  info.nrunnable  = runnableprocs();

  struct proc *p = myproc();
  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}