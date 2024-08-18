#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main()
{
    UART *my_uart = uart_create("/dev/ttyS1", BAUD_9600, 'N', 8, 1, 1, 1);


    if(!my_uart)
    {
        fprintf(stderr, "Failed to initialize UART\n");
        return EXIT_FAILURE;
    }

    printf("UART initialized successfully!\n");

    // Setting GPIO
    if(!uart_setupGPIO(my_uart, 24, "out"))
    {
        fprintf(stderr, "Failed to set up GPIO 24\n");
    }

    if (!uart_setupGPIO(my_uart, 26, "in"))
    {
        fprintf(stderr, "Failed to set up GPIO 26\n");
    }

    printf("GPIO 24 set to output mode.\n");
    printf("GPIO 26 set to input mode.\n");

    // Data to send
    const char *message = "Hello, World!";
    while (true)
    {
        int value;
        if (uart_readGPIO(my_uart, 26, &value))
        {
            printf("GPIO 26 value read: %d\n", value);
            if (value == 1)
            {
                printf("GPIO is high. Sending message: %s\n", message);
                ssize_t bytesWritten = uart_writeData(my_uart, message);
                printf("Bytes sent: %zd\n", bytesWritten);
            }
        }
        else
        {
            fprintf(stderr, "Failed to read GPIO 26 value.\n");
        }

        usleep(100000);  // Sleep for 100ms
    }


    uart_destroy(my_uart);
    return EXIT_SUCCESS;


    return 0;
}
