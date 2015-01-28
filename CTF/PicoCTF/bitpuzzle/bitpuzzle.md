# BitPuzzle

## Problem
This program seems to be a puzzle! Can you reverse-engineer it and find the solution to the puzzle?

## Writeup
This problem has a binary which when run asks for a string, and then says if the string is correct or not.

By doing a `file` on the binary, I got
`bitpuzzle: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, stripped`

It is basically a 32-bit ELF binary and is stripped. I fired up IDA pro and searched for the string "Bet" since the binary prints the string "Bet you cant solve....".
I go to the section where that string is printed. It is part of a function which is doing some interesting things. The following code snippet helped me infer what was happening.

````
mov     dword ptr [esp], offset s ; "Bet you can't solve my puzzle!"
call    _puts
mov     dword ptr [esp], offset aGiveMeAString_ ; "Give me a string."
call    _puts
mov     eax, ds:stdin
mov     [esp+8], eax    ; stream
mov     [esp+8], eax    ; stream
mov     dword ptr [esp+4], 50h ; 'P' ; n
mov     [esp], ebx      ; s
call    _fgets
````

It printed out the message, and printed out "Give me a string" and then set up the stack for a call to fgets, which is basically getting the user input. Since the first argument
is where the string is read, it is the value in ebx.

After the call, there as  snippet doing this.

````
mov     byte ptr [esp+ecx+1Ah], 0
mov     edi, ebx
mov     ecx, edx
repne scasb
cmp     ecx, 0FFFFFFDEh
jz      short loc_8048582
````

The instruction repne scasb is used to calculate the length of the string. (ecx will be decremented until a null character is encountered). ecx is then compared to -34.
Since ecx was initialized to -1, we can infer that the length is intedted to be 32 bytes. If it is not 32 bytes, the control branches and an error message is printed. So the input
is 32 bytes.

We can see segments of code like below several times.

````
mov     edx, [esp+1Ch]
mov     eax, [esp+20h]
mov     edi, [esp+24h]
lea     ebx, [edi+eax]
mov     ecx, 0
cmp     ebx, 0C0DCDFCEh
jnz     short loc_80485AD
````

The input string is stored on stack. The above snippet of code is dividing the input string into chunks of 4 bytes and doing some arithematic operations on them and comparing it
to a constant value and branching accordingly. Moving down the disassembly, if ecx is zero, the code branches to the wrong branch, else it prints "you solved my puzzle" and the input
string we give is the flag. Each segment of code does some operations and if the conditions are not met, then ecx is set to 0. Our goal is to keep ecx from becoming 0. On analysing
the segments, I got the following equations

Assuming the input string is divided into 4 byte chunks a till h:

````
1. b+c = 3235700686
2. b + a = -707535396
3. 5*b + 3*a = 1078621798
4. (a ^ d) = 0x18030607
5. (d & a) = 0x666C6970
6. e * b = 0xB180902B
7. c * e = 0x3E436B5F
8. e + 2 * f = 0x5C483831
9. (f & 0x70000000) = 0x70000000
10. f/g = 1
11. f % g = 0xE000CEC
12. 3 * e + 2 * h = 0x3726EB17
13. 7 * h + 4 * c != 0x8B0B922D
14. d + 3 * h == 0xB9CF9C91
 ````
 
These equations may look strange since `f/g = 1` which means **f = g** but `f%g = 0xE000CEC`. We have to keep in mind that this is an ELF binary, probably written in 'c' or any
other language and there are a few constraints like max integer size, integer division, etc. The hint asks us to use a constraint solver too. I tried my luck solving equations 2 and 3
using an online equation solver and got the values of a and b. Since these are parts of a string, I wrote down the corresponding character representation.

````
a= -2308149389 0x766C6F73 vlos
b= 1600613993  0x5F676E69 _gni
````

By inserting the value of 'b' in equation 1, I also got c.

````
b+c = 3235700686
c= 1635086693 0x61757165 auqe
````

From equation 4, `(a ^ d) = 0x18030607`.
I did `d=a^(a^d)` and got the value
`d=0x6E6F6974 noit`

Now I couldn't get the value of 'e' using equation 6. This is where constraints come into picture. I tried to use **python constraints** but then ended up writing my own C code.
The code takes into account that all the characters are lower case ascii or '_' (since the previous flags were like this. I calculated all possible combinations of 4 byte lowercase ascii
values which will satisfy equation 6. Here is a snippet of my code.

````
    int b = 0x5F676E69, c = 0x61757165;
    int e;
    int result1 = 0xB180902B, result2 = 0x3E436B5F, temp;
    char x1, x2, x3, x4;
    int count1, count2, count3, count4;
    for(count4=0x5f; (count4 >= 0x5f && count4 <= 0x7a); count4++)
    {
        for(count3=0x5f; (count3 >= 0x5f && count3 <= 0x7a); count3++)
        {
            for(count2=0x5f; (count2 >= 0x5f && count2 <= 0x7a); count2++)
            {
                for(count1=0x5f; (count1 >= 0x5f && count1 <= 0x7a); count1++)
                {
                    e = count1;
                    e += (count2<<8);
                    e += (count3<<16);
                    e += (count4<<24);
                    if((b*e == result1) && (c*e == result2))
                    {
                            printf("val = %x\n", e);
                    }
                }
            }
        }
    }
````

I got the value of e to be
`e = 0x73695f73 si_s`

I used similar C program to get other values.
````
f = 0x746f6c5f tol_
g=0x666f5f73 fo_s
h = 0x6e75665f nuf_
````

Since it is a little endian machine, each chunk has to be printed in a reverse order. Hence

**FLAG: solving_equations_is_lots_of_fun**
 

 
 