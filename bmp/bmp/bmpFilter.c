#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE* parseCommandLine(int argc, char** argv, int* isGrayscale) {
    if (argc > 2) {
        printf("Usage: %s [-g]\n", argv[0]);
        exit(BAD_NUMBER_ARGS);
    }

    if (argc == 2 && strcmp(argv[1], "-g") == 0) {
        *isGrayscale = TRUE;
    }
    else {
        *isGrayscale = FALSE;
    }

    return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
    unsigned fileSizeInBytes = 0;

    rewind(stream);
    if (fseek(stream, 0L, SEEK_END) != 0) {
        exit(FSEEK_ERROR);
    }
    fileSizeInBytes = ftell(stream);

    return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
    rewind(stream);
    if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
        printf("feof() = %x\n", feof(stream));
        printf("ferror() = %x\n", ferror(stream));
#endif
        exit(FREAD_ERROR);
    }
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
    return ((int)blue + (int)green + (int)red) / 3;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
    unsigned char pixelColor = getAverageIntensity(pixel[0], pixel[1], pixel[2]);
    pixel[0] = pixelColor;
    pixel[1] = pixelColor;
    pixel[2] = pixelColor;
}

void applyThresholdToPixel(unsigned char* pixel) {
    unsigned char pixelColor = getAverageIntensity(pixel[0], pixel[1], pixel[2]);
    if (pixelColor >= 128) {
        pixel[0] = 0xff;
        pixel[1] = 0xff;
        pixel[2] = 0xff;
    }
    else {
        pixel[0] = 0x00;
        pixel[1] = 0x00;
        pixel[2] = 0x00;
    }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
    if (isGrayscale) {
        applyGrayscaleToPixel(pixel);
    }
    else {
        applyThresholdToPixel(pixel);
    }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
    for (int i = 0; i < width; ++i) {
        applyFilterToPixel(row + i * 3, isGrayscale);
    }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
    for (int i = 0; i < height; ++i) {
        applyFilterToRow(pixelArray + i * width * 3 + (width * 3) % 4, width, isGrayscale);
    }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale) {
    int offsetFirstBytePixelArray = 0;
    int width = 0;
    int height = 0;
    unsigned char* pixelArray = NULL;

    offsetFirstBytePixelArray = *((int*)(bmpFileAsBytes + 10));
    width = *((int*)(bmpFileAsBytes + 14 + 4));
    height = *((int*)(bmpFileAsBytes + 14 + 8));
    pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
    printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
    printf("width = %u\n", width);
    printf("height = %u\n", height);
    printf("pixelArray = %p\n", pixelArray);
#endif

    applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char** argv) {
    int grayscale = FALSE;
    unsigned fileSizeInBytes = 0;
    unsigned char* bmpFileAsBytes = NULL;
    FILE* stream = NULL;

    stream = parseCommandLine(argc, argv, &grayscale);
    fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
    printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

    bmpFileAsBytes = (unsigned char*)malloc(fileSizeInBytes);
    if (bmpFileAsBytes == NULL) {
        exit(MALLOC_ERROR);
    }
    getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

    parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale);

#ifndef DEBUG
    if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
        exit(FWRITE_ERROR);
    }
#endif

    free(bmpFileAsBytes);
    return 0;
}