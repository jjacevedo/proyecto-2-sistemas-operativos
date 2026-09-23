# Autoevaluación — Juan José Acevedo Otálvaro

## Contribuciones

- Implementación de `sys_trace` y `sys_sysinfo` en `kernel/sysproc.c`
- Implementación de `freepages()` en `kernel/kalloc.c`
- Implementación de `runnableprocs()` en `kernel/proc.c`
- Modificación del dispatcher `syscall()` en `kernel/syscall.c` para interceptar llamadas trazadas
- Creación de los programas de usuario `trace.c` y `sysinfo.c`
- Integración de las syscalls en `user/user.h`, `user/usys.pl` y `Makefile`
- Pruebas de compilación y ejecución en QEMU

## Calificación propuesta

**4.5 / 5**

## Justificación

Las dos syscalls funcionan correctamente. `trace` intercepta en el dispatcher central sin modificar cada syscall individualmente. `sysinfo` transfiere el struct de forma segura con `copyout()`. El único aspecto mejorable es que `traced_syscall` es global y no por proceso, lo que limita el rastreo a un solo syscall simultáneo a nivel de sistema.
