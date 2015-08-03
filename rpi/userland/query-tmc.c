// http://zwizwa.be/-/electronics/20110326-160707
// http://zwizwa.be/darcs/pool/src/tmc.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/select.h>
#include <sys/time.h>

#define BUFSIZE 1024*1024+1024
#define NEXT {argv++;argc--;}

char *dev = "/dev/usbtmc0";
unsigned char buf[BUFSIZE];

void usage_exit(void) { 
    fprintf(stderr, "usage: query-tmc [-r] <command>\n");
    fprintf(stderr, "example: query-tmc -r *IDN?\n");
    exit(1); 
}
void error_exit(void) {
    perror(dev);
    exit(1);
}

int main(int argc, char **argv) {

    int do_read = 0;
    char *cmd = NULL;
    int fd, nb;

    NEXT;
    while(argc) {
        if (!strcmp(*argv, "-r")) { do_read=1; NEXT; }
        else { cmd = *argv; goto ok; }
    }
    usage_exit();

  ok:
    
    if (-1 == (fd = open(dev, O_RDWR))) error_exit();
    if (-1 == write(fd, cmd, strlen(cmd))) error_exit();
    if (do_read) {
        if (-1 == (nb = read(fd, buf, BUFSIZE))) error_exit();
        write(1, buf, nb);
        write(1, "\n", 1);
    }
    return 0;
}

