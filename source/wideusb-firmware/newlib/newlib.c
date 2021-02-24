#include "newlib-monitor.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>

#include <stm32f4xx_hal.h>
#include "usbd_cdc_if.h"
#include "cmsis_os.h"

#define UNUSED_ARGUMENT(x) ((void) x)

//extern uint32_t __get_MSP(void);
//extern UART_HandleTypeDef UART_Handle;

int heap_used = 0;
int heap_total = 0;

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status)
{
    UNUSED_ARGUMENT(status);
    while (1);
}

int _close(int file)
{
    UNUSED_ARGUMENT(file);
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    UNUSED_ARGUMENT(name); UNUSED_ARGUMENT(argv); UNUSED_ARGUMENT(env);
    errno = ENOMEM;
    return -1;
}

int _fork()
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    UNUSED_ARGUMENT(file); UNUSED_ARGUMENT(st);
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid()
{
    return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    UNUSED_ARGUMENT(tz);
    uint32_t os_time = osKernelSysTick() * configTICK_RATE_HZ / 1000;
    tv->tv_sec = os_time / 1000;
    tv->tv_usec = (os_time % 1000) * 1000;
    return 0;
}

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    UNUSED_ARGUMENT(pid); UNUSED_ARGUMENT(sig);
    errno = EINVAL;
    return (-1);
}

int _link(char *old, char *new)
{
    UNUSED_ARGUMENT(old); UNUSED_ARGUMENT(new);
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    UNUSED_ARGUMENT(file); UNUSED_ARGUMENT(ptr); UNUSED_ARGUMENT(dir);
    return 0;
}

caddr_t _sbrk(int incr)
{
    extern char __bss_end__;
    extern char _estack;
    char* heap_limit = &_estack - 0x200;

    static char* heap_end = &__bss_end__;
    char* current_block_address;

    current_block_address = heap_end;

    // Need to align heap to word boundary, else will get
    // hard faults on Cortex-M0. So we assume that heap starts on
    // word boundary, hence make sure we always add a multiple of
    // 4 to it.
    incr = (incr + 3) & (~3); // align value to 4
    if (heap_end + incr > heap_limit)
    {
        // Some of the libstdc++-v3 tests rely upon detecting
        // out of memory errors, so do not abort here.
#if 0
        extern void abort (void);
        _write (1, "_sbrk: Heap and stack collision\n", 32);
        abort ();
#else

      errno = ENOMEM;
      return (caddr_t) -1;
#endif
    }

    heap_end += incr;

    heap_used = current_block_address - &__bss_end__;
    heap_total = heap_limit - &__bss_end__;

    return (caddr_t) current_block_address;
}

int _read(int file, char *ptr, int len)
{
    UNUSED_ARGUMENT(ptr); UNUSED_ARGUMENT(len);
    switch (file)
    {
    /*case STDIN_FILENO:
        //HAL_UART_Receive(&UART_Handle, (uint8_t *)ptr, 1, HAL_MAX_DELAY);
        return 1;*/
    default:
        errno = EBADF;
        return -1;
    }
}

int _stat(const char *filepath, struct stat *st)
{
    UNUSED_ARGUMENT(filepath);
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t _times(struct tms *buf)
{
    UNUSED_ARGUMENT(buf);
    return -1;
}

int _unlink(char *name)
{
    UNUSED_ARGUMENT(name);
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    UNUSED_ARGUMENT(status);
    errno = ECHILD;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDOUT_FILENO: /* stdout */
        CDC_Transmit_FS((uint8_t*) ptr, len);
        break;
    case STDERR_FILENO: /* stderr */
        CDC_Transmit_FS((uint8_t*) ptr, len);
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
