# Proyecto 2 — System Calls en xv6

**Sistemas Operativos — Universidad EAFIT, Semestre 2026-2**  
Juan José Acevedo Otálvaro 
Miguel Angel Marín 
Sebastian Henao

---

## ¿Qué implementamos?

Dos nuevas syscalls en el kernel de xv6-riscv:

### `trace`

Activa el rastreo de una syscall por nombre. A partir de ese momento, cada vez que cualquier proceso invoque esa syscall, el kernel imprime en consola el PID del proceso, el nombre del syscall, el valor de retorno y los registros del procesador (s0, s1, a0, a1).

```
$ trace sys_fork
rastreando sys_fork
$ ls
PID: 2 / SYSCALL: sys_fork / RETURN: 5 / s0: 0x3460 / s1: 0x3e80 / a0: 0x5 / a1: 0x0
...
```

### `sysinfo`

Devuelve información del estado actual del sistema: memoria libre, páginas usadas, páginas disponibles y procesos en estado RUNNABLE.

```
$ sysinfo
Free Memory: 127 MB
Used Pages: 232
Available Pages: 32536
Runnable Processes: 0
```

---

## Archivos modificados

| Archivo | Cambio |
|---|---|
| `kernel/syscall.h` | Números de syscall 23 (trace) y 24 (sysinfo) |
| `kernel/syscall.c` | Tabla de dispatch actualizada + lógica de interceptación de trace |
| `kernel/sysproc.c` | Implementación de `sys_trace` y `sys_sysinfo` |
| `kernel/kalloc.c` | Función `freepages()` para contar páginas libres |
| `kernel/proc.c` | Función `runnableprocs()` para contar procesos RUNNABLE |
| `kernel/defs.h` | Declaraciones de `freepages` y `runnableprocs` |
| `user/user.h` | Prototipos de `trace` y `sysinfo` para espacio de usuario |
| `user/usys.pl` | Stubs de ecall para las dos syscalls |
| `user/trace.c` | Programa de usuario para activar el rastreo |
| `user/sysinfo.c` | Programa de usuario para consultar el estado del sistema |

El repositorio contiene únicamente los archivos modificados. El resto del código base de xv6-riscv se obtiene por separado.

---

## Compilación y ejecución

Se requiere el toolchain `riscv64-unknown-elf-gcc` y QEMU con soporte RISC-V.

```bash
# Clonar xv6-riscv base
git clone https://github.com/mit-pdos/xv6-riscv.git
cd xv6-riscv

# Copiar los archivos de este repo reemplazando los originales
cp <ruta-proyecto-2>/kernel/* kernel/
cp <ruta-proyecto-2>/user/user.h user/
cp <ruta-proyecto-2>/user/usys.pl user/
cp <ruta-proyecto-2>/user/trace.c user/
cp <ruta-proyecto-2>/user/sysinfo.c user/

# Compilar y ejecutar en QEMU
make qemu
```

---

## Pruebas

Dentro del prompt de xv6:

```bash
# Probar sysinfo
$ sysinfo

# Probar trace con sys_fork
$ trace sys_fork
$ ls

# Probar trace con sys_kill
$ trace sys_kill
$ kill 99
```

Para salir de QEMU: `Ctrl+A` seguido de `X`.

---

## Decisiones de diseño

**`trace`**: se implementó con una variable global `traced_syscall[64]` en `kernel/syscall.c`. El dispatcher central `syscall()` compara el nombre del syscall ejecutado contra esta variable después de cada llamada, e imprime si coincide. Este enfoque es simple y no requiere modificar cada syscall individualmente.

**`sysinfo`**: el struct `sysinfo` se define en `kernel/sysproc.c` y se transfiere al espacio de usuario mediante `copyout()`, que realiza la copia de forma segura entre espacios de memoria. La memoria libre se calcula recorriendo la lista enlazada `kmem.freelist` de kalloc. Los procesos RUNNABLE se cuentan recorriendo el arreglo `proc[]` del kernel.

---

## Declaración de uso de IA

Se utilizaron herramientas de inteligencia artificial generativa (Claude) como apoyo en el proceso de implementación: exploración de la estructura del kernel, comprensión del mecanismo de syscalls y revisión de código. El diseño, la implementación y la comprensión conceptual son responsabilidad de los autores.