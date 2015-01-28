#include<stdio.h>

void main()
{
  /*

    //Calculate e

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

*/
/*
    //Calculate f


    int e = 0x73695f73;
    int f, f2;
    int result1 = 0x5C483831, result2 = 0x70000000;
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
                    f = count1;
                    f += (count2<<8);
                    f += (count3<<16);
                    f += (count4<<24);
                    f2 = 2*f;
                    if((e+f2 == result1) && (f&result2 == result2))
                    {
                            printf("val = %x\n", f);
                    }
                }
            }
        }
    }    

*/
/*
    //Calculate g

    int f = 0x746f6c5f;
    int g;
    int result1 = 1, result2 = 0xE000CEC;
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
                    g = count1;
                    g += (count2<<8);
                    g += (count3<<16);
                    g += (count4<<24);
                    if((f/g == result1) && (f%g == result2))
                    {
                            printf("val = %x\n", g);
                    }
                }
            }
        }
    }
*/
    //Calculate h

    int e = 0x73695f73;
    int h;
    int result1 = 0x3726EB17, result2 = 0xE000CEC;
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
                    h = count1;
                    h += (count2<<8);
                    h += (count3<<16);
                    h += (count4<<24);
                    if(((3*e) + (2*h) == result1) && (result2 == result2))
                    {
                            printf("val = %x\n", h);
                    }
                }
            }
        }
    }



}
