#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_dma.h"

extern int errno;
extern UART_HandleTypeDef huart2;

int _write(int fd, char *ptr, int len) {
    (void)fd;
    
    return len;
}

int _read(int fd, char *ptr, int len) {
    (void)fd;
    for (int i = 0; i < len; ++i) {
        HAL_UART_Receive(&huart2, (uint8_t *)&ptr[i], 1, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t *)&ptr[i], 1, HAL_MAX_DELAY);  // Echo
    }
    return len;
}

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

void _exit(int status) {
    (void)status;
    while (1) { }
}

int _kill(int pid, int sig) {
    (void)pid; (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1;
}

caddr_t _sbrk(int incr) {
    extern char _end;
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return (caddr_t)prev_heap_end;
}
