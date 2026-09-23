#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(2, "uso: trace <nombre_syscall>\n");
    exit(1);
  }

  if (trace(argv[1]) < 0) {
    fprintf(2, "trace: error al activar rastreo de %s\n", argv[1]);
    exit(1);
  }

  fprintf(1, "rastreando %s\n", argv[1]);
  exit(0);
}
