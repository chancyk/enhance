#include <stdio.h>
#include <stdlib.h>

#define InstrBits(x)     ((char)x & (char)0b11111100)  // shave off the last two bits
#define MoveModeBits(x)  ((char)x & (char)0b11000000)  // top two bits are the move mode
#define MoveRegValue(x)  (((char)x & (char)0b00111000) >> 3)
#define MoveRmValue(x)   (((char)x & (char)0b00000111))

const char INSTR_MOVE__D_BIT = (char)0b00000010;
const char INSTR_MOVE__W_BIT = (char)0b00000001;
const char INSTR_MOVE = (char)0b10001000;
const char INSTR_MOVE__MODE_REGREG = (char)0b11000000;


typedef enum StateEnum {
    STATE_INITIAL,
    STATE_MOVE
} State;

const char *REGISTERS[] = {
    "a", // 000
    "c", // 001
    "d", // 010
    "b", // 011
    "xx", // 100
    "xx", // 101
    "xx", // 110
    "xx"  // 111
};

void print_binary(int bits, char byte, const char *end){
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

    fp = fopen("../computer_enhance/perfaware/part1/listing_0038_many_register_mov", "r");
    if (fp == NULL) {
        printf("Failed to open file.");
        exit(1);
    }

    //print_binary(0b00100010);
    //printf("%d\n", (char)0b10001001);

    State state = STATE_INITIAL;
    char buffer[64];
    char instr_byte = 0;
    while (1) {
        int bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (bytes_read == 0) {
            break;
        }
        //printf("READ [-]: %d\n", bytes_read);
        if (bytes_read < 2)
        {
            printf("Expected at least 2 bytes.");
            goto EXIT;
        }

        for (int i = 0; i < bytes_read; i++)
        {
            //printf("BYTE [%d]: ", i); print_binary(8, buffer[i], "\n");
            if (state == STATE_INITIAL) {
                    if (InstrBits(buffer[i]) == INSTR_MOVE)
                    {
                        //printf("CMD  [%d]: ", i); print_binary(6, InstrBits(buffer[i]), "dw\n");
                        state = STATE_MOVE;
                        instr_byte = buffer[i];
                    }
            }
            else if (state == STATE_MOVE) {
                char payload = buffer[i];
                //printf("MOVE [%d]: ", i); print_binary(8, payload, "\n");
                if (MoveModeBits(payload) == INSTR_MOVE__MODE_REGREG) {
                    // reg is the destination
                    char *dest;
                    char *src;
                    int is_word = 0;
                    if (instr_byte & INSTR_MOVE__W_BIT) {
                        is_word = 1;
                    }
                    if (instr_byte & INSTR_MOVE__D_BIT)
                    {
                        src = REGISTERS[MoveRmValue(payload)];
                        dest = REGISTERS[MoveRegValue(payload)];
                    }
                    else {
                        src = REGISTERS[MoveRegValue(payload)];
                        dest = REGISTERS[MoveRmValue(payload)];
                    }
                    if (is_word) {
                        //printf("INST [%d]: ", i);
                        printf("move %sx, %sx\n", dest, src);
                    }
                    else {
                        //printf("INST [%d]: ", i);
                        printf("move %sl, %sl\n", dest, src);
                    }
                }
                state = STATE_INITIAL;
            }
        }
    }

EXIT:
    fclose(fp);
}
