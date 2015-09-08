#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>

extern void add_protection();
extern int remove_protection();

int *protector;
char *name_buf;

int fortune_teller(char *input);
int secret();
void error();
void print_fortune(int fortune);
 
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
    int fortune = 0; //execute = 1;
    char input_date[12], input_city[15], name[25];
    char *year;    
   
    while(1)
    {
        printf("*********************Welcome to Caesar's Hut.************************\n\n\n");
        printf("We wish you a pleasant stay in Vegas.\n");
        printf("Please complete the registration process before you begin playing at the casino\n\n\n");
        printf("Please enter your name\n");
        fgets(name, 25, stdin);
        name_buf = (char *)calloc(strlen(name), sizeof(char));
        strcpy(name_buf, name);

        printf("Please enter the city in which you were born in\n");
        fgets(input_city, 15, stdin);

        printf("Enter your date of birth (MM/DD/YYYY)\n");
        fgets(input_date, 12, stdin);

        //To do: Add code to enter the collected values into database

        //Parse the year of birth
        year = strtok(input_date, "/");
        year = strtok(NULL, "/");
        year = strtok(NULL, "/");
        if(!year)
        {
            printf("Please enter the date of birth in the following format (MM/DD/YYYY)\n");
            continue;
        }
        
        //Check if "special user"
        memset(buf,0, 9);
        strncpy(buf, name, 3);
        strncat(buf, input_city, 4);

        //Add year to the special user check
        strcat(buf, year);
        if(!(fortune_teller(buf)))
        {
            fortune = 1;
        }

        if(fortune == 1)
        {
            printf("secret\n");
            //CFI checks. (Not sure what it does but was told to add)
            protector = malloc(sizeof(int));
            *protector = 0;

            secret();
            free(protector);
        }
        else
        {
            printf("Welcome again. You have been successfully registered for playing. Please head to the counter to get you chips. Good Luck\n\n");
            fortune = rand() % 5; //5 different fortunes
            print_fortune(fortune);
            printf("-----------------====================================================================================---------------------\n\n");
        }
        free(name_buf);
    }
    return 0;
}

int secret()
{
    add_protection();
    char buf[16]; 
    char filename[16];
    FILE *f;// = fopen("message.txt", "w");
    int uid = getuid();   
 
    printf("U are in the secret arena now. Enter your nick name. \n");
    fgets(name_buf, 25, stdin);
    
    printf("Enter the dROP location.\n");
    gets(buf);

    sprintf(filename, "/tmp/%d.txt", uid);
    f = fopen(filename, "w");
    fprintf(f, "Message by: %s%s\n",name_buf, buf);
    fclose(f);
    
    printf("The message has been stored. Get away from here before anyone recognizes you\n\n\n");
    
    if(remove_protection())
    {
        error();
    }
    return 0;
    
}

int fortune_teller(char *input)
{
    char secret_hash[] = "183444c2c3fe22a142532952904bafff";
    unsigned char digest[16];
    MD5_CTX context;
    
    size_t len = strlen(input);
    MD5_Init(&context);
    MD5_Update(&context, input, len);
    MD5_Final(digest, &context);
    char md5string[33];
    for(int i = 0; i < 16; ++i)
    {
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);
    }
    
    return !(!(strcmp(md5string, secret_hash)));
    //printf("%s\n", md5string);
}

void print_fortune(int fortune)
{
    
    printf("Your stars today say:\n");
    //Add switch case
    switch(fortune)
    {
        case 0:
            printf("Dont worrt about the money. The best things in life are free\n");
            break;
        case 1:
            printf("Borrow money from pessimists. They dont expect it back\n");
            break;
        case 2:
            printf("Cross the ocean to find the riches\n");
            break;
        case 3:
            printf("An oppurtunity at hand is better than two in the bush\n");
            break;
        case 4:
            printf("You've got to risk it for the biscuit\n");
            break;
    }
}

void error()
{
    printf("Possible buffer overflow detected\n");
    exit(0);
}
