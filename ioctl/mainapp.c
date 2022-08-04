#include <stdio.h>
#include <string.h>
#include <sys/types.h> //for open()
#include <fcntl.h> //open() flag parameter macros, e.g. O_RDWR
#include <unistd.h>//for close(), and open()
#include <sys/ioctl.h> //ioctl(int fd, unsigned long request, ...)

/*
fd is a file descriptor
    file descriptors are unique positive integers which represent an opened file for the process
request is a code which specifies the request function to be called
third argument is untyped pointer to memory
*/

#include "query_ioctl.h"

void get_vars(int fd){
    query_arg_t q;

    if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1){
	perror("query_apps ioctl get");
    }
    else{
	printf("Var_1 : %d\n", q.var_1);
	printf("Var_2 : %d\n", q.var_2);
	printf("Var_3 : %d\n", q.var_3);
    }
}
void clr_vars(int fd){
    if (ioctl(fd, QUERY_CLR_VARIABLES) == -1){
	perror("query_apps ioctl clr");
    }
}
void set_vars(int fd)
{
    int v;
    query_arg_t q;

    printf("Enter var_1: ");
    scanf("%d", &v);
    getchar();
    q.var_1 = v;
    printf("Enter var_2: ");
    scanf("%d", &v);
    getchar();
    q.var_2 = v;
    printf("Enter var_3: ");
    scanf("%d", &v);
    getchar();
    q.var_3 = v;

    if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1){
	perror("query_apps ioctl set");
    }
}

int main(int argc, char *argv[]){
    char *file_name = "/dev/query";
    int fd;
    enum{
	e_get,
	e_clr,
	e_set
    } option; //create enum for better readability, set option based on what argv[1] is

    if (argc == 1){
	option = e_get;
    }
    else if (argc == 2){
	if (strcmp(argv[1], "-g") == 0){
	    option = e_get;
	}
	else if (strcmp(argv[1], "-c") == 0){
	    option = e_clr;
	}
	else if (strcmp(argv[1], "-s") == 0){
	    option = e_set;
	}
	else{
	    fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
	    return 1;
	}
    }
    else{
	fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
	return 1;
    }
    fd = open(file_name, O_RDWR); 
    //open(const char *file_name, int flags)
    //creates and returns a file descriptor for the given file
    if (fd == -1){
	perror("query_apps open");
	return 2;
    }

    switch (option){
	case e_get:
	    get_vars(fd);
	    break;
	case e_clr:
	    clr_vars(fd);
	    break;
	case e_set:
	    set_vars(fd);
	    break;
	default:
	    break;
    }

    close (fd);
    return 0;
}
