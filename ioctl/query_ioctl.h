#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H

#include <linux/ioctl.h> //for _IO* macros

typedef struct{
    int var_1, var_2, var_3;
} query_arg_t;

#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

#endif

//QUERY_GET_VARIABLES: "get" variables from the device file to user space
//QUERY_CLR_VARIABLES: set all variables to zero
//QUERY_SET_VARIABLES: set device file variables to userspace variables 

/*
IO* are helper macros to create unique identifiers
so that the right command does not get issued to the wrong device
parameters:
    magic #
    command id
    type of data to be copied
*/
