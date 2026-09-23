#include "kernel/types.h"
#include "user/user.h"

struct sysinfo {
  uint64 freemem;
  uint64 usedpages;
  uint64 availpages;
  int    nrunnable;
};

int
main(void)
{
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    fprintf(2, "sysinfo: error\n");
    exit(1);
  }

  fprintf(1, "Free Memory: %ld MB\n",   info.freemem);
  fprintf(1, "Used Pages: %ld\n",       info.usedpages);
  fprintf(1, "Available Pages: %ld\n",  info.availpages);
  fprintf(1, "Runnable Processes: %d\n", info.nrunnable);

  exit(0);
}
