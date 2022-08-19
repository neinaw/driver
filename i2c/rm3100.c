#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>
#include<sys/types.h>

#define FILE "/dev/i2c-0"
#define ADD 0x20 + 1 << 1 + 1 //slave address 0x20 + SA1 << 1 + SA0

int main(){
    int fd; 
    char buf[10];
    char ans_x[3];
    char ans_y[3];
    fd = open(FILE, O_RDWR);
    ioctl(fd, I2C_SLAVE, ADD);

    //poll to the device
    buf[0] = 0x00; //for POLL address reg
    buf[1] = 0b00110000; //request which axes to be measured
    write(fd, buf, 2); 
    
    //check status register
    char s = 0xB4;
    char reply;
    write(fd, &s, 1);
    do{
	read(fd, &reply, 1);
    } while(reply >> 7 == 1);

    char read_results;

    read_results = 0xA4;
    write(fd, &read_results, 1);
    read(fd, ans_x, 3);
    read(fd, ans_y, 3);

    int X_fin = (ans_x[2] << 16) + (ans_x[1] << 8) + ans_x[0];
    int Y_fin = (ans_y[2] << 16) + (ans_y[1] << 8) + ans_y[0];

    return 0;
}
