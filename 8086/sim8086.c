#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#if DEBUG
#define dbg(x) __builtin_dump_struct(&(x), fprintf, stderr)
#define debug(text) fprintf(stderr, (text))
#define debugf(fmt, ...) fprintf(stderr, (fmt), __VA_ARGS__)
#else
#define dbg(...)
#define debug(...)
#define debugf(...)
#endif


#define InstrBits(x)     ((char)x & (char)0b11111100)  // shave off the last two bits
#define MoveModeBits(x)  ((char)x & (char)0b11000000)  // top two bits are the move mode
#define MoveRegValue(x)  (((char)x & (char)0b00111000) >> 3)
#define MoveRmValue(x)   (((char)x & (char)0b00000111))


const char INSTR_MOVE = (char)0b10001000;
const char INSTR_MOVE__D_BIT = (char)0b00000010;
const char INSTR_MOVE__W_BIT = (char)0b00000001;
const char INSTR_MOVE__MODE_REGREG = (char)0b11000000;


typedef enum StateEnum {
    STATE_INITIAL,
    STATE_MOVE
} State;

const char *BYTE_REGISTERS[] = {
    "al", // 000
    "cl", // 001
    "dl", // 010
    "bl", // 011
    "ah", // 100
    "ch", // 101
    "si", // 110
    "di"  // 111
};

const char *WORD_REGISTERS[] = {
    "ax", // 000
    "cx", // 001
    "dx", // 010
    "bx", // 011
    "sp", // 100
    "bp", // 101
    "si", // 110
    "di"  // 111
};


void print_binary(int bits, char byte, const char *end){
    debug("0b");
    while (bits--)
    {
        if (byte & (char)0b10000000)
            debug("1");
        else
            debug("0");

        byte <<= 1;
    }
    debug(end);
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
    debugf("%d\n", (char)0b10001001);
    printf("bits 16\n\n");

    State state = STATE_INITIAL;
    int num_instr = 19;
    char buffer[64];
    char instr_byte = 0;
    while (1) {
        int bytes_read = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (bytes_read == 0) {
            break;
        }
        debugf("READ [-]: %d\n", bytes_read);
        if (bytes_read < 2)
        {
            printf("Expected at least 2 bytes.");
            goto EXIT;
        }

        for (int i = 0; i < bytes_read; i++)
        {
            debugf("BYTE [%2d]: ", (i % 2) + 1); print_binary(8, buffer[i], "\n");
            if (state == STATE_INITIAL) {
                    if (InstrBits(buffer[i]) == INSTR_MOVE)
                    {
                        debugf("CMD  [%d]: ", num_instr); print_binary(6, InstrBits(buffer[i]), "dw\n");
                        state = STATE_MOVE;
                        instr_byte = buffer[i];
                    }
            }
            else if (state == STATE_MOVE) {
                char payload = buffer[i];
                debugf("MOVE [%d]: ", num_instr); print_binary(8, payload, "\n");
                if (MoveModeBits(payload) == INSTR_MOVE__MODE_REGREG) {
                    // reg is the destination
                    const char *dest;
                    const char *src;
                    int is_word = (instr_byte & INSTR_MOVE__W_BIT) ? 1 : 0;
                    if (is_word) {
                        if (instr_byte & INSTR_MOVE__D_BIT)
                        {
                            src = WORD_REGISTERS[MoveRmValue(payload)];
                            dest = WORD_REGISTERS[MoveRegValue(payload)];
                        }
                        else {
                            src = WORD_REGISTERS[MoveRegValue(payload)];
                            dest = WORD_REGISTERS[MoveRmValue(payload)];
                        }
                    }
                    else {
                        if (instr_byte & INSTR_MOVE__D_BIT)
                        {
                            src = BYTE_REGISTERS[MoveRmValue(payload)];
                            dest = BYTE_REGISTERS[MoveRegValue(payload)];
                        }
                        else {
                            src = BYTE_REGISTERS[MoveRegValue(payload)];
                            dest = BYTE_REGISTERS[MoveRmValue(payload)];
                        }
                    }

                    debugf("INST [%d]: ", num_instr);
                    printf("move %s, %s\n", dest, src);
                }
                state = STATE_INITIAL;
                num_instr++;
            }
        }
    }

EXIT:
    fclose(fp);
}
