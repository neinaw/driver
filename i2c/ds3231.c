/* elinux.org/Interfacing_with_I2C_Devices */
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdint.h>
#include<sys/ioctl.h>
#include<linux/i2c-dev.h>
#include<sys/stat.h>

#define DEV "/dev/i2c-0"
#define DEST "out.txt"

uint8_t addr = 0b01101110; //slave address
uint8_t sec = 0x00; //seconds register
uint8_t min = 0x01; //minutes register
uint8_t hour = 0x02; //hours register
uint8_t date = 0x04; //date register
uint8_t ibyte = 0x11; //integer part of time
uint8_t fbyte = 0x12; //fractional part of time

int cur_date = 0;
char time[20];
float temp;

ssize_t my_write(int fd, const void *buf, size_t count){
    ssize_t ans = write(fd, buf, count);
    if(ans == count) return ans;
    else{
	perror("Write Error\n");
	return ans;
    }
}

ssize_t my_read(int fd, void *buf, size_t count){
    ssize_t ans = read(fd, buf, count);
    if(ans == count) return ans;
    else{
	perror("Read Error\n");
	return ans;
    }
}

int my_pow(int exp, int base){
    if(base == 0 && exp == 0) return -1;
    return exp ? base * my_pow(exp-1,base) : 1;
}

int bcd_to_decimal(uint8_t arg){
    int ans = 0;
    for(int i=0;i<8;++i){
	if(i<=3){
	    ans += my_pow(i, 2) * ((arg >> i) % 2);
	}
	else{
	    ans += 10 * (my_pow(i-4, 2) * ((arg >> i) % 2));
	}
    }
    return ans;
}

int get_date(){
    int fd;
    uint8_t data;
    uint8_t reply;

    fd = open(DEV, O_RDWR);

    if(ioctl(fd, I2C_SLAVE, addr) < 0){
	perror("Failed to talk to slave\n");
	close(fd);
	return -1;
    }

    data = date;
    my_write(fd, &data, 1);
    my_read(fd, &reply, 1);
    close(fd);

    cur_date = bcd_to_decimal(reply);
    return 0;
}

int get_time(){
    int fd;
    uint8_t data[3] = {sec, min, hour};
    uint8_t reply[3];
    fd = open(DEV, O_RDWR);

    if(ioctl(fd, I2C_SLAVE, addr) < 0){
	perror("Failed to talk to slave\n");
	close(fd);
	return -1;
    }

    for(int i=0;i<3;i++){
	my_write(fd, data+i,1);
	my_read(fd, reply+i,1);
    }
    close(fd);

    if((reply[3] >> 5) % 2){
	int s,m,h;
	s = bcd_to_decimal(reply[2]);
	m = bcd_to_decimal(reply[2]);
	if((reply[2] >> 4) % 2 == 1){
	    h = bcd_to_decimal(data[2] ^ 0b00110000);
	    snprintf(time, 14,"%dh:%dm:%ds a.m.",h,m,s);
	} else { 
	    h = bcd_to_decimal(data[2] ^ 0b00100000);
	    snprintf(time, 10,"%dh:%dm:%ds",h,m,s);
	}
    } 
    else{
	int s,m,h;
	s = bcd_to_decimal(reply[2]);
	m = bcd_to_decimal(reply[2]);
	h = bcd_to_decimal(data[2]);
	snprintf(time, 10,"%dh:%dm:%ds",h,m,s);
    }
    return 0;
}

int get_temp(){
    int fd;
    uint8_t data[2] = {ibyte, fbyte};
    int i_part;
    unsigned f_part;

    fd = open(DEV, O_RDWR);

    if(ioctl(fd, I2C_SLAVE, addr) < 0){
	perror("Failed to talk to slave\n");
	close(fd);
	return -1;
    }

    my_write(fd, data, 1);
    my_read(fd, &i_part, 1);
    my_write(fd, data+1, 1);
    my_read(fd, &f_part, 1);
    close(fd);
    
    temp = (float)i_part;
    f_part >>= 6; 
    for(int i=0;i<2;i++){
	temp += ((f_part >> i) % 2) * (1 / my_pow(i+1,2));
    }
    return 0;
}

int get_size(){
    struct stat info;
    stat(DEST, &info);
    return info.st_size;
}

int main(){
    FILE *fp = fopen(DEST, "w+");
    get_date();
    get_time();
    get_temp();
    while(get_size() < 1000){
	fprintf(fp, "Date:\t%d\nTime:\t%s\nTemperature:\t%f\n",cur_date, time, temp);
	sleep(10);
    }
    printf("File size exceeded 1000kB.\n");
    return 0;
}
