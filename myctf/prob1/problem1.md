# Anti-ROP

## Problem area
Binary exploitation

## Problem statement
There have been intelligence inputs that the casino **Caesar's Hut** is involved in drug smuggling activities. It has been suspected that the casino registration 
application at the counter is used by suppliers and dealers for sharing details of various deals. There may be a mechanism to store secret messages inside the harmful looking
registration app. So your mission is to hack into the application and read the secret messages (flag) stored. There was an insider who managed to obtain one of the source files for us to analyze. 
The guy who developed this application claims that this application is pretty secure. In addition to have ASLR and DEP, it includes special functions which perform 
Control Flow Integrity checks making buffer-overflow attacks difficult. However, there may still be various holes in the application which could be exploited. 
Good Luck.

## Hint
Arbitrary memory writes are powerful. Also one could guess the layout of the heap if only one had a base address.

## Skills required
Basic buffer overflow concepts, format string vulnerabilities and Return Oriented Programming

## Program details
This program is supposed to be a registration screen for the casino. The program, when started prints the welcome screen and then asks for name of the user followed by 
city in which the user was born and the date of birth. On entering these details, the user gets a combination of the name and city as username which is supposedly used at 
the casino counter for registration. Also based on the input, the program tells the user today's fortune and wishes him/her luck at the casino.
Also, on entering a particular date and city combination, a secret screen unlocks. The hash of the secret combination is stored in a file to which the access is restricted. 
When the user enters his/her details, it is hashed and then compared with the stored hash, and if successful, the secret screen is unlocked, where the user is asked to 
enter a secret name and a message. Both the secret name and the message is stored in a file 'message.txt' which again has restricted permissions.

## Description/Walkthrough
This is a multi-level exploitation problem with increasing difficulty.
If we check the source, the name and city we enter are concatenated in a certain way and the string is directly passed to `printf`. This is the first point where there 
is a basic format string vulnerability present. Also, in the source, there is a pointer on the stack called MAGIC which is used to store a `malloc`'ed integer. 
This is a hint for the hacker to read the pointer MAGIC on the stack and thus know the address of the `malloc`'ed chunk. This will be useful later in the program.

However, the program then does some computations on the date and city and decides whether the `is_secret` flag is set or not and thus the control passing to the secret screen or not.
Our goal is to make this flag which is currently 0, to become 1. The flag becomes 1, if the hash of our input combinations matches with the hash present in the `credentials` file. 
However, this is not a problem related to hashing and trying to brute force the combo to make the flag 1. On verifying the source, the inputs are all restricted using fgets to their 
buffer sizes and hence we cant give long inputs. The city and the date buffers are concatenated to generate the hash. The date uses `strtok` to extract the year from the MM/DD/YYYY
format. As per the logic used, even M/D/YYYYYY is a valid input, which is enough to overwrite the `is_secret` variable on stack and set it to 1. We can now go to the secret phase 
of the program.

This part accepts a secret name again and writes it to a file using `fprintf`, once more, without proper formatting. This can be leveraged to perform arbitrary read/writes. Also,
there is a buffer overflow staring at us when the program reads the message as input and writes it to a file. This is an unrestricted read, but since the binary has ASLR and DEP 
enabled, it is not as straightforward.

Here is where things get interesting. One might start thinking that a ROP chain could be used to bypass DEP and ASLR limitations. However, there is are two special functions 
`add_protection` and `remove_protection` called after entering the `secret` function and before leaving, respectively. On examining the disassembly, these functions perform 
Control Flow Integrity checks. There is a global pointer `protector` pointing to a location on the heap. This is initialized before calling `add_protection`. The function then goes
ahead and stores the value of return address that the function is supposed to return to on the heap, away from all the buffer overflow madness. The function `remove_protection` 
performs a simple comparision between the value the function is going to return, and the actual return value stored on the heap. If this Control Flow Integrity check doesn't pass, 
the program simply exits printing "Possible buffer overflow detected". These functions actually work and upon giving a ROP chain, since the return address is changed, the program 
exits.

If only there was a way to overwrite the "super safe" location on the heap pointed to by `protector`. Here is where the two format string vulnerabilities come into play.
We know that the fprintf is ready to do our arbitrary memory write. We are all set to write the value of the address of the first ROP gadget (the return address which we want) to the
value on the heap. But how do we find the location of that chunk since heap is randomized. Remember the first vulnerability which promised us to read an arbitrary memory location
and there was a malloc'ed chunk pointed to conveniently by MAGIC? Thus we get the location of a chunk on the heap, so what? We didnt get the value of the chunk storing the 
return address. However, since both are integers, and libc malloc has a minimum block size of 16 bytes, we can guess that the next block malloced would be at 16 bytes from *MAGIC, 
unless no other blocks were freed. This is exactly the scenario.

Hence we now have the address of the chunk containing the return address of `secret` function. We can overwrite it using our second format string vulnerability with a value equal to
the start of our ROP chain and then let the ROP chain run. 

Here is the flow of exploits:
1. Set `is_secret` to 1 by using a flaw in the strtok logic
2. Overflow the buffer using a ROP chain since ASLR and DEP is enabled, only to be blocked by special CFI functions `add_protection` and `remove_protection`
3. Realize the need to overwrite the chunk the CFI functions use to store the return address using second format string vulnerability present in `secret`
4. Realize that we don't know the address on the heap where the return address is stored due to heap randomization
5. Go back to `main` and exploit the first format string vulnerability to read the location of MAGIC which is the first malloced block on the heap
6. Use that location to calculate the location of second malloc'ed block which is `protector` and overwrite it using second format string vulnerability
7. Overflow the buffer with the awesome ROP chain
8. **Get a shell**

Phewww, finally its time to execute `cat flag.txt`.


