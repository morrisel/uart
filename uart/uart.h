#ifndef UART_H
#define UART_H

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include "bool_defs.h"

enum BaudRate
{
    BAUD_2400 = B2400,
    BAUD_9600 = B9600,
    BAUD_19200 = B19200,
    BAUD_38400 = B38400,
    BAUD_57600 = B57600
};
#define BaudRate enum BaudRate


struct UART
{
    BaudRate baudRate;
    char parity;
    uint32_t databits;
    uint32_t stopbits;
    uint32_t vmin;
    uint32_t timeout;
    int fd;
    bool isOpen;
    struct termios uart;
};
#define UART struct UART

UART* uart_create(const char    *path,
                  BaudRate      baudRate,
                  char          parity,
                  uint32_t      databits,
                  uint32_t      stopbits, 
                  uint32_t      vmin,
                  uint32_t      timeout);

void uart_destroy(UART *uart);

bool uart_isOpen(UART *uart);

ssize_t uart_writeData(UART *uart, const char *buff);

ssize_t uart_readData(UART *uart, char *buff, uint32_t sizeRead);



bool uart_setupGPIO(UART *uart, int pin, const char *direction);

bool uart_readGPIO(UART *uart, int pin, int *value);

bool uart_writeGPIO(UART *uart, int pin, int value);



#endif
