#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1

/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE* parseCommandLine(int argc, char** argv, int* bits) {
    if (argc > 2) {
        printf("Usage: %s [-b|-bits]\n", argv[0]);
        exit(BAD_NUMBER_ARGS);
    }

    if (argc == 2 &&
        (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
        *bits = TRUE;
    }
    else {
        *bits = FALSE;
    }

    return stdin;
}


/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char* data, size_t size) {
    for (int i = 0; i < 16; ++i) {
        if ((i % 2) == 0) {
            printf(" ");
        }
        if (i >= size) {
            printf("  ");
        }
        else if (data[i] != '\n') {
            printf("%x", data[i]);
        }
        else {
            printf("0a");
        }
    }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char* data, size_t size) {
    for (int i = 0; i < 16; ++i) {
        if (i >= size)
        {
            printf(" ");
        }
        else if (data[i] != '\n') {
            printf("%c", data[i]);
        }
        else {
            printf("%c", '.');
        }
    }
}

void printDataAsBinary(unsigned char* data, size_t size) {
    for (int i = 0; i < 6; ++i) {
        int decimal = data[i];
        char convert[8];
        printf(" ");
        if (i >= size) {
            printf("00000000");
        }
        else if (data[i] == '\n') {
            printf("00001010");
        }
        else {
            for (int j = 7; j >= 0; --j) {
                if (decimal == 0) {
                    convert[j] = '0';
                }
                else {
                    if (decimal % 2 == 1) {
                        convert[j] = '1';
                    }
                    else {
                        convert[j] = '0';
                    }
                    decimal /= 2;
                }
            }
            for (int k = 0; k < 8; ++k) {
                printf("%c", convert[k]);
            }
        }
    }
}

void readAndPrintInputAsHex(FILE* input) {
    unsigned char data[16];
    int numBytesRead = fread(data, 1, 16, input);
    unsigned int offset = 0;
    while (numBytesRead != 0) {
        printf("%08x:", offset);
        offset += numBytesRead;
        printDataAsHex(data, numBytesRead);
        printf("  ");
        printDataAsChars(data, numBytesRead);
        printf("\n");
        numBytesRead = fread(data, 1, 16, input);
    }
}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE* input) {
    unsigned char data[6];
    int numBytesRead = fread(data, 1, 6, input);
    unsigned int offset = 0;
    while (numBytesRead != 0) {
        printf("%08x:", offset);
        offset += numBytesRead;
        printDataAsBinary(data, numBytesRead);
        printf("  ");
        printDataAsChars(data, numBytesRead);
        printf("\n");
        numBytesRead = fread(data, 1, 6, input);
    }
}

int main(int argc, char** argv) {
    int bits = FALSE;
    FILE* input = parseCommandLine(argc, argv, &bits);

    if (bits == FALSE) {
        readAndPrintInputAsHex(input);
    }
    else {
        readAndPrintInputAsBits(input);
    }
    return 0;
}