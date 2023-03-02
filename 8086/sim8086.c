#include <stdio.h>
#include <stdlib.h>

#define InstrBits(x) ((char)x & (char)0b11111100)  // shave off the last two bits

const char INSTR_MOVE = (char)0b10001000;


typedef enum StateEnum {
    STATE_INITIAL,
    STATE_MOVE
} State;


void print_binary(int bits, char byte, const char* end) {
    printf("0b");
    while (bits--)
    {
        if (byte & (char)0b10000000)
            printf("1");
        else
            printf("0");

        byte <<= 1;
    }
    printf(end);
}


int main(int argc, char* argv) {
    FILE *fp;
    char ch;

    fp = fopen("../computer_enhance/perfaware/part1/listing_0037_single_register_mov", "r");
    if (fp == NULL) {
        printf("Failed to open file.");
        exit(1);
    }

    //print_binary(0b00100010);
    //printf("%d\n", (char)0b10001001);

    State state = STATE_INITIAL;
    char buffer[64];
    while (1) {
        int bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (bytes_read == 0) {
            break;
        }
        printf("READ [-]: %d\n", bytes_read);
        if (bytes_read < 2)
        {
            printf("Expected at least 2 bytes.");
            goto EXIT;
        }

        for (int i = 0; i < bytes_read; i++)
        {
            printf("BYTE [%d]: ", i); print_binary(8, buffer[i], "\n");
            if (state == STATE_INITIAL) {
                    if (InstrBits(buffer[i]) == INSTR_MOVE)
                    {
                        printf("CMD  [%d]: ", i); print_binary(6, InstrBits(buffer[i]), "dw\n");
                        state = STATE_MOVE;
                    }
            }
            else if (state == STATE_MOVE) {
                printf("MOVE [%d]: ", i); print_binary(8, buffer[i], "\n");
            }
        }
    }

EXIT:
    fclose(fp);
}
