# ROP ME IF YOU CAN

## Problem area
Binary exploitation

## Problem statement
There have been intelligence inputs that the casino Caesar's Hut is involved in drug smuggling activities. It has been suspected that the casino registration application 
at the counter is used by suppliers and dealers for sharing details of various deals. There may be a mechanism to store secret messages inside the harmless looking 
registration app. So your mission is to hack into the application and read the secret messages (flag) stored. There was an insider who managed to obtain one of the source 
files for us to analyze. The guy who developed this application claims that this application is pretty secure. In addition to having ASLR and DEP, it includes special functions 
which perform Control Flow Integrity checks making buffer-overflow attacks difficult. However, there may still be various holes in the application which could be exploited. 
Good Luck.

## Hint
The hashing algorithm might be perfect. Look for other avenues. The function add_protection() shares the same memory space as the process. 

## Skills required
Basic buffer overflow concepts and Return Oriented Programming

## Program details
This program is supposed to be a registration screen for the casino. The program, when started prints the welcome screen and then asks for name of the user followed by 
city in which the user was born and the date of birth. These details are then supposed to go into the database. Also the program calculates a random number tells the user today's 
fortune and wishes him/her luck at the casino.
On entering a particular input combination, a secret screen unlocks. The user input is formatted accordingly and checked against a stored hash. If the hash matches, there is a secret
screen unlocked which is used for storing messages about the illegal activities. There is a flaw in the "user-input-hash-creation" logic which can be exploited to go to the secret screen. 
There is a vulnerability in this stage of the program too, which when exploited, gives a shell.

## Description/Walkthrough
On checking the source, the first three characters from the name, four from the city and the year part of the Date Of Birth are concatenated and passed to a function `fortune_teller`.
This function compares the MD5 hash of the input passed with a pre-stored hash. One might be inclined towards cracking the hash using Rainbow Tables and other means but the hint
reveals that there might be a vulnerability elsewhere.
On examining the source again, the year is parsed from the DOB using `strtok` and then `strcat` and NOT `strncat` is used to append the year part of the string which is compared
against the hash. There is a possibility of an overflow here. The program assumes that the date is entered in the correct format "MM/DD/YYYY". If "YYYY" part is null, it produces an
error. However, it is not checking the length of each field, (month, date, or the year). Hence, this is also a valid input: **1/1/1234567890**. On giving this input to the program, 
and examining the stack, it can be seen that the input is truncated at '7' and a '\0' NULL character is appended. However, there are no checks, and the value overflows variables 
above this buffer. The variable `fortune` is set to '1' if the hash is correct. Hence, passing the input such that the last charecter is '\x01' would set it to one, bypassing the 
hash check.

On entering the secret screen, the program again asks for name, and then the message. These are written into a temp file as "stored secret messages". The message input uses `gets` which
is a dead giveaway. However, the binary is ASLR and DEP enabled. Creating a ROP chain is not so much effective because of the function `add_protection()` which when called, stores the 
value of the return address in a location on the heap, and the function `remove_protection()` that is called in the end, compares the current return address with the value stored on 
the heap and throws an error if they dont match. Hence ROP is not possible. However, the return address saved by `add_protection` is on the heap. If that value can be overwritten, 
the secret function could return to anywhere it wants.
If one observes keenly, the buffer on the heap created for name in the first part of the program used `strlen` and malloced the size based on user-input. However, in the secret part,
25 bytes are read into the same buffer. This is again a heap-overflow. Since the `name_buf` and `protector` variable which is used by `add_protection` function are malloced consecutively,
they are allocated adjacent memory blocks. Hence, if the name entered in the first part is small enough so that only 8 bytes are allocated, the name in the second part can successfully
overwrite the protector variable, rendereing the `add_protection` checks ineffective. Now, a ROP chain can be inputted as the message and cause it to get a shell.
Refer to the rop_exploit.py program for the actual exploit code

## Flag
`shadow_stack_makes_life_harder`


