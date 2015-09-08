# Will you handle me!!!!
=============

## Area
-----
Binary exploit 

## Skill level
-----------
Intermediate/Expert

## Knowledge required
------------------
Basic buffer overflow
Signals

## Resources
---------
https://www.cs.vu.nl/~herbertb/papers/srop_sp14.pdf

http://thisissecurity.net/2015/01/03/playing-with-signals-an-overview-on-sigreturn-oriented-programming/

## Educational Value
------------------
Signal handlers, what to include and what not

The way signal return mechanism could be exploited if not coded properly

## Problem setup
-------------
- NX

## Problem statement
---------
Deadalus Corp has a secret vault which can be opened with a preset secret password. Once the console to access the vault is activated, a user would have 10 seconds to enter the pass-phrase,
or else the console turns itself off and has to be restarted. Inside the vault lies the secret formula to take over the world. Get into the vault and find the secret. Remember, the
future of the world depends on you.

## Concepts involved
---------------------
Signal handling
Sigreturn/SROP
Buffer overflow

This problem dives into the working of signal handlers, especially how a handler returns control to user level program. This mechanism can be exploited to change the flow of 
execution.

When a signal handler is invoked, the kernel switches control from the user program to the signal handler. Before doing this, the user context is stored on the stack which is to
be restored later. When a signal handler returns, it returns to a stub which executes sigreturn system call. When sigreturn executes, the kernel restores the context of the user 
process and then the control is passed back to user process. The structure into which the context is stored is referred to as "sigcontext" or "uccontext". This structure contains the 
value of all general purpose registers along with a few other registers. Controlling this structure would imply control of all the registers including EIP after the execution of
sigreturn.

## Walk through
-------------

1) The program has a countdown of 10 seconds. On examining the code, there is a signal handler registered for SIGINT inside which there is code to ask the user for the password.
Thus the user has to hit CTRL-C. When a user does that, a prompt appears asking the user to input the password. This password is compared with a pre-calculated hash and if it is a match, the vault is opened revealing
its contents.

2) The password is read using the `fgets` function. On observing carefully, `fgets` is using `BUF_LEN` to read into the password buffer. This is a vulnerability since the buffer is of `PASS_LEN` which is 17 bytes
but `BUF_LEN` is 512 bytes. (Input any value to test the behaviour).
````
fgets(input, BUF_LEN, stdin);
````
This easy to spot vulnerability inside signal handler gives the user a chance to overwrite the values on the stack. On further examining the code, there are calls to `open_vault`, `add_protection` and `remove_protection`
in the signal handler. Hence the aim is to control the EIP and point it to `open_vault` function which does the work required to get the flag.

3) The user cannot directly overwrite the return address of the signal handler. This is because of the `remove_protection` function. On checking the disassembly, there are two functions that are used 
to add additional security against control hijacking attacks. There is a global variable called `protector` in the code which points to a location on the heap. 
````
 65     protector = malloc(sizeof(int));
````
The `add_protection` function takes the value of ebp+4, which would be return address of the caller function and stores it in `protector`. The `remove_protection` takes that value and
restores it just before the caller function returns. Thus this eliminates traditional return address overwrite attacks. However, there is one more method to exploit this program which is described in the next step.

4) The return address on the stack frame of the signal handler points to `sigreturn` system call stub. Above the return address is the struct sigcontext. This sigcontext structure contains
the saved values of most of the registers including EIP. The `sigreturn` call uses this structure to restore the context for the program when it returns from the signal handler.
The sigcontext structure has the values of registers saved just before entering the signal handler. Also, the address of the instruction where the signal occurred and where the control should
return after the execution of handler is saved (EIP). If the user changes the saved value of EIP in the sigcontext structure, the user can control the execution of the program when 
the signal handler returns. However, one needs to be careful about changing the stack pointer, frame pointer etc. The safest way is to change only the value of EIP.

5) On running the program under gdb and putting the break point just at the return statement of the signal handler, one can view the stack at that point when the breakpoint hits.
Note that gdb catches all the signals by default, and hence gdb should be instructed to not catch the signal but to pass it to the program.
````
handle SIGINT nostop pass
````
The attacker can then compare the values on the stack to the fields of the structure sigcontext, or one can proceed with the execution to check where it returns to and compare it 
with the values on the stack at breakpoint to locate the address of saved EIP. Also, it is safer to note the other values on the stack of the sigcontext structure. Now one can
construct an exploit string using python.

6) After calculating the offsets, the exploit string is needs to be in the following format.
````
37 bytes of junk (which includes 4 bytes of return address which will be overwritten by remove_protection) + saved sigcontext values of other registers  + EIP of open_vault
````
The final exploit string is as follows
````
python -c 'print "A"*37 + "\x02\x00\x00\x00" + "\x63\x00\x00\x00" + "\x00\x00\x00\x00" + "\x2b\x00\x00\x00" + "\x2b\x00\x00\x00" + "\x44\xd5\xff\xff" + "\x00\x00\x00\x00" + "\xc4\xd4\xff\xff" + "\x7c\xd4\xff\xff" + "\xbc\xd4\xff\xff" + "\x00\xb0\xe1\xf7" + "\xbc\xd4\xff\xff" + "\xfc\xff\xff\xff" + "\x01\x00\x00\x00" + "\x00\x00\x00\x00" + "\x32\x89\x04\x08" + "\x23\x00\x00"' > in.txt
/home/iVault/iVault < in.txt
````

This transfers the control to open_vault function which prints the flag for us. The flag is 
**i_will_be_careful_with_sigreturn**

