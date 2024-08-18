
#include "uart.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


UART* uart_create(const char    *path,
        BaudRate      baudRate,
        char          parity,
        uint32_t      databits,
        uint32_t      stopbits,
        uint32_t      vmin,
        uint32_t      timeout)
{
    UART *uart = (UART*)malloc(sizeof(UART));
    if (!uart)
    {
        return NULL;
    }

    uart->baudRate=      baudRate;
    uart->parity=        parity;
    uart->databits=      databits;
    uart->stopbits=      stopbits;
    uart->vmin=          vmin;
    uart->timeout=       timeout;
    uart->fd=       -1;
    uart->isOpen=       false;


    uart->fd = open(path, O_RDWR | O_NOCTTY);
    if(uart->fd ==-1)
    {
        free(uart);
        return NULL;
    }

    uart->isOpen=true;

    // Initialize device
    if (tcgetattr(uart->fd, &uart->uart))
    {
        close(uart->fd);
        free(uart);
        return NULL;
    }

    cfsetispeed(&uart->uart, uart->baudRate);
    cfsetospeed(&uart->uart, uart->baudRate);

    uart->uart.c_cflag &= ~PARENB;     // Clear parity enable
    uart->uart.c_cflag &= ~CSTOP;      // Clear stop bits
    uart->uart.c_cflag &= ~CSIZE;      // Clear character size bits

    uart->uart.c_iflag = IGNBRK;       // 
    uart->uart.c_lflag = 0;            // 
    uart->uart.c_oflag = 0;            // 

    uart->uart.c_cflag |= ~PARENB;     // Set parity bits
    uart->uart.c_cflag |= ~CSTOP;      // Set stop bits
    uart->uart.c_cflag |= ~CSIZE;      // Set data bits

    uart->uart.c_iflag &= ~(IXON | IXOFF | IXANY);

    uart->uart.c_cc[VTIME] = uart->timeout;
    uart->uart.c_cc[VMIN] = uart->vmin;

    uart->uart.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(uart->fd, TCSANOW, &uart->uart))
    {
        close(uart->fd);
        free(uart);
        return NULL;
    }

    return uart;

}


void uart_destroy(UART *uart)
{
    if (uart)
    {
        if(uart->isOpen)
        {
            close(uart->fd);
        }
        free(uart);
    }
}



bool uart_isOpen(UART *uart)
{
    return uart && uart->isOpen;
}

ssize_t uart_writeData(UART *uart, const char *buff)
{
    if (!uart || !uart->isOpen)
    {
        return -1;
    }
    return write(uart->fd, buff, strlen(buff));
}

ssize_t uart_readData(UART *uart, char *buff, uint32_t sizeRead)
{
    if (!uart || !uart->isOpen)
    {
        return -1;
    }
    return read(uart->fd, buff, sizeRead);
}


bool uart_setupGPIO(UART *uart __attribute__((unused)) , int pin, const char *direction)
{
    (void)uart;
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    FILE *gpio_dir = fopen(path, "w");
    if(!gpio_dir)
    {
        return false;
    }
    fprintf(gpio_dir, "%s", direction);
    fclose(gpio_dir);
    return true;
}

bool uart_readGPIO(UART *uart __attribute__((unused)) , int pin, int *value)
{
    (void)uart;
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    FILE *gpio_value = fopen(path, "r");
    if (!gpio_value)
    {
        return false;
    }
    fscanf(gpio_value, "%d", value);
    fclose(gpio_value);
    return true;
}

bool uart_writeGPIO(UART *uart __attribute__((unused)), int pin, int value)
{
    (void)uart;
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);

    FILE *gpio_value = fopen(path, "w");
    if(!gpio_value)
    {
        return false;
    }
    fprintf(gpio_value, "%d", value);
    fclose(gpio_value);
    return true;
}







