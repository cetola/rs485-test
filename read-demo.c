#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */
 
int main(void) {
    struct serial_rs485 rs485conf_sp0;
    char buf[8] = {0};
    int n = 0;
    int sp0 = 0;
    struct termios tio;
    
    memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = CS8 | CREAD | CLOCAL;
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;
    
    cfsetospeed(&tio, B115200);
     
    sp0 = open ("/dev/ttySP4", O_RDWR);
    if (sp0 < 0) {
       printf("Error: Can't open: /dev/ttySP0\n");
       goto e_close;
    }

    /* Read the current state of the RS-485 options with ioctl. */
    if (ioctl (sp0, TIOCGRS485, &rs485conf_sp0) < 0) {
            printf("Error: TIOCGRS485 ioctl not supported.\n");
    }

    /* Enable RS-485 mode: */
    rs485conf_sp0.flags |= SER_RS485_ENABLED;

    /* Set rts/txen delay before send, if needed: (in microseconds) */
    rs485conf_sp0.delay_rts_before_send = 0;
 
    /* Set rts/txen delay after send, if needed: (in microseconds) */
    rs485conf_sp0.delay_rts_after_send = 0;
 
    /* Write the current state of the RS-485 options with ioctl. */
    if (ioctl (sp0, TIOCSRS485, &rs485conf_sp0) < 0) {
            printf("Error: TIOCSRS485 ioctl not supported.\n");
    }

    /* set the termios so the rs485 settings are applied */
    tcsetattr(sp0, TCSANOW, &tio);
    fcntl(sp0, F_SETFL, 0);
    
    int i = 0;
    while(i < 5) {
        printf("reading 'ABC' from SP4 \n");
        char readbuf[5];
        n = read(sp0, &buffer, 5);
        if (n < 0) {
          printf("error reading from SP4");
        } else {
          printf("%c", buffer[0]);
          printf("%c", buffer[1]);
          printf("%c\n", buffer[2]);
        }
        ++i; 
    }


    printf("closing SP4\n");
    if (close (sp0) < 0) {
            printf("Error: Can't close: /dev/ttySP4\n");
    }

e_close:
    if(sp0) {
        close(sp0);
    }

    return 0;
}
