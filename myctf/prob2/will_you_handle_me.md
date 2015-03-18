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

## Problem setup
-------------
- NX 
- ASLR

## Problem statement
---------
Deadalus Corp has a secret vault which can be opened with a preset secret password. Once the console to access the vault is activated, a user would have 50 seconds to enter the pass-phrase,
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

1) The program has a countdown of 50 seconds. On examining the code, there is a signal handler registered for SIGINT inside which there is code which asks the user for the password.
This leads the user to hit ctrl-c which gives a prompt to enter a password. This password is compared with a pre-calculated hash and if it is a match, the vault is opened revealing
its contents.

2) The buffer in which the password is being stored is not bound by any length. This easy to spot vulnerability inside signal handler gives one a chance to overwrite the values on the 
stack. When inside signal handler, the stack contains return address which points to sigreturn stub. Above that is the struct sigcontext.

3) An important thing to keep in mind is that there is a SIGSEGV handler which just prints "Something is wrong" and exits immediately. Hence the struct sigcontext has to be overwritten 
with care, overwriting only the EIP and keeping other relevant values which may cause a segmentation fault intact.

4) Now we have the control of EIP. This can be pointed to `grant_access` which gives a shell, hence granting access to "flag.txt" .