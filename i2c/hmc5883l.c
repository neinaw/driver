#include <stdint.h>
#include <stdio.h>         // Standard Input and Output
#include <fcntl.h>         // for open() function
#include <unistd.h>        // for close() and sleep() functions
#include <sys/ioctl.h>     // contains ioctl() function prototype and related macros
#include <linux/i2c-dev.h> // Allows I2C bus communication using read() and write()

#define I2C_DEVICE_FILE "/dev/i2c-0"
uint8_t I2C_SLAVE_ADDRESS 0x1E
uint8_t I2C_SLAVE_READ 0x3D
uint8_t I2C_SLAVE_WRITE 0x3C
char buf[10]; // For all read and write operations

int measure()
{
  buf[0]=I2C_SLAVE_WRITE; // Request write
  buf[1]=0x02; // Mode Register
  buf[2]=0x01; // Value for Single Measurement mode

  if (write(file,buf,3)!=3) // Request a single measurement
  {
    printf("Failed to write to the Mode Register.\n");
    return -1;
  }

  buf[0]=I2C_SLAVE_READ; // Request read
  buf[1]=0x09; // Status Register
  write(file, buf, 2); // Ask for reading
  read(file, buf, 1); // Read Status Register
  if ((buf[0]%2)!=1) // Check Status
  {
    printf("Measurement Failed (Status Register 0)\n");
    return -1;
  }
  
  buf[0]=I2C_SLAVE_READ; // Request read
  buf[1]=0x03; // X MSB Register
  write(file, buf, 2); // Request readings
  read(file, buf, 6); // Read measurement registers
  int X_reading = (buf[0]<<8)+(buf[1]); // X-axis measurement
  int Y_reading = (buf[4]<<8)+(buf[5]); // Y-axis measurement
  int Z_reading = (buf[2]<<8)+(buf[3]); // Z-axis measurement

  printf("X-axis Reading: %d\n",X_reading);
  printf("Y-axis Reading: %d\n",Y_reading);
  printf("Z-axis Reading: %d\n",Z_reading);
  
  return 0;
}

int main()
{
  int file;

  // Opening the I2C Device File
  if ((file=open(I2C_DEVICE_FILE, O_RDWR))<0) // Allow read and write operations
  {
    printf("Failed to open I2C device file.\n");
    return -1;
  }

  // Connecting to the Sensor (I2C Slave)
  if (ioctl(file, I2C_SLAVE, I2C_SLAVE_ADDRESS)<0)
  {
    printf("Failed to connect to sensor (ioctl).\n");
    return -1;
  }

  while(1)
  {
    measure();
    sleep(2);
  }

  close(file); // Close the I2C Device File
  return 0;
}
