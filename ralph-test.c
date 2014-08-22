#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */
 
int main(int argc, char *argv[])
{
    struct serial_rs485 rs485conf;
    char buf[8] = {0};
    int n = 0;
    int ttysp = 0;        // default to /dev/ttySP0
    struct termios tio;
    char rs485_mode = 0;
    char tty_string[40];
    char opt_string[40];
    char msg[80];
 
    // parse command line
    if (argc > 1)
    {
        // printf("argv[1]: %s\n", argv[1]);    // REM - for debug
        if (strcmp(argv[1], "0") || strcmp(argv[1],"4"))
        {
            ttysp = atoi(argv[1]);
        }
        else
        {
            printf("Only 0 or 4 are valid for tty port\n");
            return(1);
        }
    }
    if (argc > 2)
    {
        // printf("argv[2]: %s\n", argv[2]);    // REM - for debug
        rs485_mode = 1;
    }
    if (argc > 3)
    {
        // printf("argv[3]: %s\n", argv[3]);    // REM - for debug
        sprintf(opt_string, "%s", argv[3]);
    }

    memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = CS8 | CREAD | CLOCAL;
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;
 
    cfsetospeed(&tio, B115200);
 
    sprintf(tty_string, "/dev/ttySP%1d", ttysp);
    // printf("tty_str: >%s<\n", tty_string);        // REM - for debug
    ttysp = open (tty_string, O_RDWR);
    if (ttysp < 0) {
       printf("Error: Can't open: %s\n", tty_string);
       goto e_close;
    }
 
    if (rs485_mode)
    {
        printf("Setting RS485 mode\n");        // REM - for debug

        /* Read the current state of the RS-485 options with ioctl. */
        if (ioctl (ttysp, TIOCGRS485, &rs485conf) < 0) {
            printf("Error: TIOCGRS485 ioctl not supported.\n");
        }
 
        /* Enable RS-485 mode: */
        rs485conf.flags = rs485_mode;
 
        /* Set rts/txen delay before send, if needed: (in microseconds) */
        rs485conf.delay_rts_before_send = 0;
 
        /* Set rts/txen delay after send, if needed: (in microseconds) */
        rs485conf.delay_rts_after_send = 0;
 
        /* Write the current state of the RS-485 options with ioctl. */
        if (ioctl (ttysp, TIOCSRS485, &rs485conf) < 0) {
            printf("Error: TIOCSRS485 ioctl not supported.\n");
        }
    }
 
    /* set the termios so the rs485 settings are applied */
    printf("settermios\n");
    int setatt = tcsetattr(ttysp, TCSANOW, &tio);
    printf("SETFL\n");
    int setfl = fcntl(ttysp, F_SETFL, 0);
    printf("setatt = %d\n", setatt);
    printf("setfl = %d\n", setfl);

    printf("writing to %s\n", tty_string);
    sprintf(msg, "t \"This is another test on %s\" 50 50\r", tty_string);
    n = write(ttysp, msg, strlen(msg));
    if (n < 0) {
       printf("error writing to %s", tty_string);
    }
    if (argc > 3)
    {
        sprintf(msg, "t \"opt_string: %s\" 50 75\r", opt_string);
        n = write(ttysp, msg, strlen(msg));
        if (n < 0) {
            printf("error writing opt_string to %s", tty_string);
        }
    }

    printf("closing tty\n");
    if (close (ttysp) < 0) {
            printf("Error: Can't close: %s\n", tty_string);
    }
 
e_close:
    if(ttysp) {
        close(ttysp);
    }
 
    return 0;
}


