#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

char traced_syscall[64];

int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if (addr >= p->sz || addr + sizeof(uint64) > p->sz)
    return -1;
  if (copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  if (copyinstr(p->pagetable, buf, addr, max) < 0)
    return -1;
  return strlen(buf);
}

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0: return p->trapframe->a0;
  case 1: return p->trapframe->a1;
  case 2: return p->trapframe->a2;
  case 3: return p->trapframe->a3;
  case 4: return p->trapframe->a4;
  case 5: return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

void
argint(int n, int *ip)
{
  *ip = argraw(n);
}

void
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
}

int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  argaddr(n, &addr);
  return fetchstr(addr, buf, max);
}

extern uint64 sys_fork(void);
extern uint64 sys_exit(void);
extern uint64 sys_wait(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_kill(void);
extern uint64 sys_exec(void);
extern uint64 sys_fstat(void);
extern uint64 sys_chdir(void);
extern uint64 sys_dup(void);
extern uint64 sys_getpid(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_pause(void);
extern uint64 sys_uptime(void);
extern uint64 sys_open(void);
extern uint64 sys_write(void);
extern uint64 sys_mknod(void);
extern uint64 sys_unlink(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_close(void);
extern uint64 sys_sync(void);
extern uint64 sys_trace(void);
extern uint64 sys_sysinfo(void);

static uint64 (*syscalls[])(void) = {
  // clang-format off
  [SYS_fork]    sys_fork,
  [SYS_exit]    sys_exit,
  [SYS_wait]    sys_wait,
  [SYS_pipe]    sys_pipe,
  [SYS_read]    sys_read,
  [SYS_kill]    sys_kill,
  [SYS_exec]    sys_exec,
  [SYS_fstat]   sys_fstat,
  [SYS_chdir]   sys_chdir,
  [SYS_dup]     sys_dup,
  [SYS_getpid]  sys_getpid,
  [SYS_sbrk]    sys_sbrk,
  [SYS_pause]   sys_pause,
  [SYS_uptime]  sys_uptime,
  [SYS_open]    sys_open,
  [SYS_write]   sys_write,
  [SYS_mknod]   sys_mknod,
  [SYS_unlink]  sys_unlink,
  [SYS_link]    sys_link,
  [SYS_mkdir]   sys_mkdir,
  [SYS_close]   sys_close,
  [SYS_sync]    sys_sync,
  [SYS_trace]   sys_trace,
  [SYS_sysinfo] sys_sysinfo,
  // clang-format on
};

static const char *syscall_names[] = {
  // clang-format off
  [SYS_fork]    "sys_fork",
  [SYS_exit]    "sys_exit",
  [SYS_wait]    "sys_wait",
  [SYS_pipe]    "sys_pipe",
  [SYS_read]    "sys_read",
  [SYS_kill]    "sys_kill",
  [SYS_exec]    "sys_exec",
  [SYS_fstat]   "sys_fstat",
  [SYS_chdir]   "sys_chdir",
  [SYS_dup]     "sys_dup",
  [SYS_getpid]  "sys_getpid",
  [SYS_sbrk]    "sys_sbrk",
  [SYS_pause]   "sys_pause",
  [SYS_uptime]  "sys_uptime",
  [SYS_open]    "sys_open",
  [SYS_write]   "sys_write",
  [SYS_mknod]   "sys_mknod",
  [SYS_unlink]  "sys_unlink",
  [SYS_link]    "sys_link",
  [SYS_mkdir]   "sys_mkdir",
  [SYS_close]   "sys_close",
  [SYS_sync]    "sys_sync",
  [SYS_trace]   "sys_trace",
  [SYS_sysinfo] "sys_sysinfo",
  // clang-format on
};

void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num]();

    if (traced_syscall[0] != '\0' &&
        num < NELEM(syscall_names) &&
        syscall_names[num] != 0 &&
        strncmp(traced_syscall, syscall_names[num], 64) == 0) {
      printk("PID: %d / SYSCALL: %s / RETURN: %ld / s0: 0x%lx / s1: 0x%lx / a0: 0x%lx / a1: 0x%lx\n",
        p->pid,
        syscall_names[num],
        p->trapframe->a0,
        p->trapframe->s0,
        p->trapframe->s1,
        p->trapframe->a0,
        p->trapframe->a1);
    }
  } else {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}