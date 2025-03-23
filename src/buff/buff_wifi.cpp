#include "buff_wifi.h"

BUFFWifi myBUFFWifi;

/* Reads a word from the buffer at specified position ------------------------*/
int BUFFWifi::Buff__getByte(int index)
{
    // The first and second characters of the byte stored in the buffer
    // are supposed to be in range ['a'; 'p'], otherwise it isn't a image data's byte
    if ((Buff__bufArr[index    ] < 'a') || (Buff__bufArr[index    ] > 'p')) return -1;
    if ((Buff__bufArr[index + 1] < 'a') || (Buff__bufArr[index + 1] > 'p')) return -1;

    // The character 'a' means 0, the character 'p' means 15 consequently,
    // The 1st character describes 4 low bits if the byte and the 2nd one - 4 high bits 
    // return ((int)Buff__bufArr[index] - 'a') + (((int)Buff__bufArr[index + 1] - 'a') << 4);
    return (((int)Buff__bufArr[index] - 'a') << 4) + (((int)Buff__bufArr[index + 1] - 'a') & 0x0f);
}

/* Reads a byte from the buffer at specified position ------------------------*/
int BUFFWifi::Buff__getWord(int index)
{
    // Read low byte of the word
    int a = Buff__getByte(index); 

    // If it is not a image data byte, then exit
    if (a == -1) return -1;

    // Read high byte of the word
    int b = Buff__getByte(index + 2);

    // If it is not a image data byte, then exit
    if (b == -1) return -1;

    // Return the word's value
    return a + (b << 8);
}

/* Reads a byte from the buffer at specified position ------------------------*/
int BUFFWifi::Buff__getN3(int index)
{
    return (index + 3 > Buff__SIZE) ? 0 :
        (Buff__bufArr[index    ]      ) + 
        (Buff__bufArr[index + 1] <<  8) + 
        (Buff__bufArr[index + 2] << 16);
}

/* Checks if the buffer's data ends with specified string --------------------*/
int BUFFWifi::Buff__signature(int index, char*str)
{
    index = 0; // temp add
    // characters of the string to the end of the string
    while (*str != 0)
    {
        // If the correspondent character in the buffer isn't equal
        // to the string's character, return false    
        if (Buff__bufArr[index++] != *str) return false;
        str++;
    }

    // Otherwise return true
    return true;
}