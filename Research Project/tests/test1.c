#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int fun1(char *buf)
{
  char a,b,c;

  a = buf[8];
  b = 10;
  c = buf[0];
  buf[6] = b;
  buf[2] = 9;
  buf[4] = a;
  buf[9] = 0;
  return 0;
}

int main(int ac, char **av)
{
  int fd;
  char *buf;

  if (!(buf = malloc(32)))
    return -1;

  fd = open("./file.txt", O_RDONLY);
  read(fd, buf, 32), close(fd);
  fun1(buf);
}
