#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <openssl/md5.h>
 
#define PASS_LEN 17
#define BUF_LEN 512

void handle_signal(int signum);
void open_vault();
extern void add_protection();
extern int remove_protection();

int *protector;

void handle_signal(int signum)
{
    add_protection();
    char input[PASS_LEN];
    char pass[] = "183444c2c3fe22a142532952904bafff";
    unsigned char digest[16];
    MD5_CTX context;
    int i;

    printf("Please enter the password to unlock the vault\n");
    fgets(input, BUF_LEN, stdin);
    strtok(input, "\n");
    MD5_Init(&context);
    MD5_Update(&context, input, 16);
    MD5_Final(digest, &context);
    char md5string[33];
    for(i = 0; i < 16; ++i)
    {
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);
    }

    if(strcmp(pass, md5string) == 0)
    {
        open_vault();
        exit(0);
    }
    else
    {
        printf("Sorry. The password is incorrect\n");
    }
    remove_protection();
    
}

void open_vault()
{
    char flag[35];
    FILE *fp;
    fp = fopen("./flag.txt", "r");
    fgets(flag, 35, fp);
    puts(flag);
}
 
int main()
{
    int remaining_time = 10;
    signal(SIGINT, (void *)handle_signal);

    protector = malloc(sizeof(int));
    *protector = 0;

    printf("Please unlock the vault with in 10 seconds\n");
    while(remaining_time > 0)
    {
        printf("Time Remaining: %2d\r", remaining_time);
        fflush(stdout);
        sleep(1);
        remaining_time--;
    }
    printf("The vault is locked again for security reasons\n");
    return 0;
}

/*int wait()
{
    clock_t end_wait;
    end_wait = clock() + CLK_TCK;
    while(clock() < end_wait){}
}*/
 
/* struct definition for debugging purpose */
struct sigcontext sigcontext;
