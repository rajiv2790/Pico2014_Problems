#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void add_protection();
extern int remove_protection();

int *protector;

int secret();
void error();
 
int main(int argc, char *argv[])
{
/*    char *buf1, *buf2, *buf3;
    if (argc != 4) return;
 
    buf1 = malloc(256);
    strcpy(buf1, argv[1]);
 
    buf2 = malloc(strtoul(argv[2], NULL, 16));
 
    buf3 = malloc(256);
    strcpy(buf3, argv[3]);
 
    free(buf3);
    free(buf2);
    free(buf1);
*/
//r < <(python -c 'print "delhi\n" + "4/4/99999\x01\n"') 
    char buf[9];
    char *year = "9999";
    int is_secret = 1;
    char input_date[12], input_city[15], name[25], username[25], temp[25];
    int eip = 0xdeadface;
    int execute = 0;
       
    int *magic = malloc(sizeof(int));    
    *magic = eip;
    
    while(1)
    {
        if(execute)
        {
        printf("Please enter your name\n");
        fgets(name, 25, stdin);
        printf("Please enter the city in which you were born in\n");
        fgets(input_city, 15, stdin);
        printf("Enter your date of birth (MM/DD/YYYY)\n");
        fgets(input_date, 11, stdin);
        
        year = strtok(input_date, "/");
        year = strtok(NULL, "/");
        year = strtok(NULL, "/");
        if(!year)
        {
            printf("Please enter the date of birth in the following format (MM/DD/YYYY)\n");
        }
        
        strcpy(username, name);
        printf("Welcome again. You have been successfully registered for playing. Please use the following user name at the counter to get you chips. Good Luck\n");
        printf(username);
        memset(buf,0, 9);
        strncpy(buf, input_city, 4);
        strcat(buf, year);
        
        if(strcmp(buf, "delh9999") == 0)
        {
            is_secret = 1;
        }
        }
        if(is_secret)
        {
        printf("secret\n");
        secret(username);
        fgets(temp, 25, stdin);
        }
        else
        {
        printf("not secret\n");
        }
    }
    return 0;
}

int secret()
{
    protector = malloc(sizeof(int));
    *protector = 0;
    add_protection();
    char buf[16], name[25];
    FILE *f = fopen("message.txt", "w");
    
    printf("U are in the secret arena now. Enter your secret name. \n");
    fgets(name, 25, stdin);
    fprintf(f, name);
    fclose(f);
    
    printf("Enter the message. \n");
    gets(buf);
    f = fopen("message.txt", "a");
    fprintf(f, "%s\n", buf);
    fclose(f);
    
    if(remove_protection())
    {
        error();
    }
    free(protector);
    return 0;
    
}

void error()
{
    printf("Possible buffer overflow detected\n");
    exit(0);
}
