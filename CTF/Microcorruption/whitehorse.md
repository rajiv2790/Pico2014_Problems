# Whitehorse

## Problem

````
OVERVIEW

    - This lock is attached to the LockIT Pro HSM-2.
    - We have updated  the lock firmware to connect with this hardware
      security module.
````

This is the first problem where HSM-2 (Hardware Security Module) was introduced.

HSM-2 is different from the HSM-1 which was present in the earlier problems. The new module does not have the familiar `unlock_door` function. Hence our exploit can't perform a 
return to that function. HSM-2 stores the password initially, compares it with the user input and directly unlocks the door or takes no action accordingly. This is achieved by 
`conditional_unlock_door` function which calls an interrupt number `0x7E`, which on referring the manual, turned out to be the interrupt to unlock the door if the user-supplied 
password is correct. There is another interrupt with an interrupt number `0x7F` which just unlocks the door, without performing any checks which is worth taking note of.

**Summary**

The input buffer length is unchecked which leads to a buffer-overflow. The return address to where the `login` function returns can be overwritten with the address of a call to 
`INT`. `INT` reads its arguments (the interrupt number) from the stack. If an interrupt number of 0x7F is passed, the door can be unlocked without any conditions (refer the manual 
entry for `0x7F` interrupt).

**Analysis**

The login routine prints the "Enter password" statement followed by a call to `getsn`, leaving the stack vulnerable. Then there is a call to `conditional_unlock_door`, and if it 
returns 0, "That password is not correct" is printed. The next step is to check the disassembly for the `conditional_unlock_door` function. This function passes the interrupt number 
(0x7E) onto the stack, and then calls `INT` which triggers a hardware interrupt. 
````

445c:  3012 7e00      push	#0x7e
4460:  b012 3245      call	#0x4532 <INT>
4464:  5f44 fcff      mov.b	-0x4(r4), r15
**
446e:  3041           ret

````
The interrupt, on returning, would have set a fixed byte in the memory to be non-zero, which is moved into r15 and then returned. The login routine checks for this value in r15 
as mentioned earlier, and expects it to be non-zero if the password entered is correct.

There is more than one approach to solve this. The main idea is that the interrupt number is passed via the stack, and the input length is not checked. What if the return 
address of the login function can be overwritten with the user-supplied buffer, and cause it to return to the `INT` routine? Then the the value of **0x7F** could be passed onto the 
stack which directly unlocks the door, without any checks.

I first put a breakpoint at the end of the login function (at ret instruction) and passed "abcdefgh". Here is how the stack looked when the execution hit the breakpoint.

````

3ec0:   0000 0000 0000 0000 4645 0100 4645 0300   ........FE..FE..
3ed0:   ba45 0000 0a00 0000 2a45 6162 6364 6566   .E......*Eabcdef
3ee0:   6768 0000 0000 0000 0000 3c44 0000 0000   gh........<D....
3ef0:   *						  |--------------------------------------------------> sp

````
**Exploitation**

The input string I entered (abcdefgh) is at location 3eab. I need 8 more bytes to reach the return address which I will be overwriting with the address of `INT`.
The padding would be of 16 bytes length. From the disassembly, the address of `INT` routine was pretty easy to find. It was `0x4532`. Note that since this is little-endian, 
the address would have to be in reverse order in the exploit string. Also, inside `INT`, the value push into the SR before the call to 0x10 is taken from stack pointer +2

````
4532 <INT>
4532:  1e41 0200      mov	0x2(sp), r14
4536:  0212           push	sr
4538:  0f4e           mov	r14, r15
453a:  8f10           swpb	r15
453c:  024f           mov	r15, sr
453e:  32d0 0080      bis	#0x8000, sr
4542:  b012 1000      call	#0x10
````

This means that the value 7F has to be after a gap of 2 bytes after the return address. Hence the final exploit string would be

16 bytes of padding + ret address in little endian (3245) + 2 bytes of padding + "7F".

**Final exploit string**
`61616161616161616161616161616161324561617f`

**NOTE: **I chose to return to the address `4532` which is the first instruction of `INT`. However, one can return to address `4554` which is a call to `INT`. If this method is chosen,
then the exploit string needs to be adjusted accordingly as the argument 7F is expected just above the return address (without 2 bytes of padding).