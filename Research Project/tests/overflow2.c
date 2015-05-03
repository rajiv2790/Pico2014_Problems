#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void give_shell(){
    gid_t gid = getegid();
    setresgid(gid, gid, gid);
    system("/bin/sh -i");
}

void vuln(){

    char buf[16];
    int fd;
    int secret = 0;
    printf("Read from file\n");
    
    fd = open("./input.txt", O_RDONLY);
    read(fd, buf, 32);
    close(fd);

    if (secret == 0xc0deface){
        give_shell();
    }
    else{
        printf("The secret is %x\n", secret);
    }
}

int main(int argc, char **argv){
        
    vuln();
    return 0;
}

