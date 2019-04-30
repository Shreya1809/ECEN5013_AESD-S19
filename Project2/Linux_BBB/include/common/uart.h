#ifndef UART_H_
#define UART_H_

int UART_config(void);
void UART_close(int fd);
void UART_iflush(int fd);
void UART_oflush(int fd);
int UART_printChar(int fd,char c);
int UART_write(int fd,char *data, size_t size);
int UART_read(int fd, char *data, size_t size);

#endif