#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <cstdlib>
#include <cctype>
#include <cstring>

const int MaxComLen = 10;
const int MaxArgLen = 32;

struct lineIndex {
    char *startIndex;
    char *endIndex;
};

int getFileSize(const char *inPath);

int readFile(const char inPath[], char *text, size_t textSize);

int nRows(const char *str, size_t textSize, char element);

void fillIndex(lineIndex *index, char *text, size_t textSize);

lineIndex *readTextFromFile(const char inPath[], char *text, size_t *textSize, size_t *rows);

int main() {
    char inPath[FILENAME_MAX] = "../program-v1.asm";
    char outPath[FILENAME_MAX] = "../program-v1.bin";

    FILE *binFile = fopen(outPath, "wb");

    size_t programSize = 0;
    size_t lines = 0;
    char *buffer = nullptr;

    lineIndex *index = readTextFromFile(inPath, buffer, &programSize, &lines);

    char *binBuffer = (char *) calloc(lines * 5, sizeof(char));
    char *bufferStart = binBuffer;

    for (size_t i = 0; i < lines; i++) {
        char str[MaxComLen] = "";
        char arg[MaxArgLen] = "";
        sscanf(index[i].startIndex, "%s%s", str, arg);

#define DEF_CMD(name, num, argType, code) \
            if (strcmp(str, #name) == 0) { \
                if (argType == 0) { \
                    sprintf(binBuffer, "%c", num); \
                    binBuffer++; \
                } \
                else { \
                    if (isdigit(*arg)) { \
                        sprintf(binBuffer, "%c", num); \
                        binBuffer++; \
                        \
                        *(int *) binBuffer = atoi(arg); \
                        binBuffer += sizeof(int); \
                    } \
                    else if (isalpha(*arg)) { \
                        char *xPointer = arg; \
                        for (xPointer; *xPointer != '\0'; xPointer++) {} \
                        xPointer--; \
                        \
                        if (*xPointer == 'x') { \
                            sprintf(binBuffer, "%c", num + 10); \
                            binBuffer++; \
                            sprintf(binBuffer, "%c", *(xPointer - 1)); \
                            binBuffer++; \
                        }\
                    }\
                } \
            } \
            else
            #include "commands.h"
            printf("Syntax error\n");
#undef DEF_CMD
    }

    fwrite(bufferStart, sizeof(char), binBuffer - bufferStart, binFile);

    free(buffer);
    fclose(binFile);
    free(bufferStart);
    return 0;
}

int getFileSize(const char *inPath) {
    assert(inPath != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }

    fseek(myFile, 0, SEEK_END);
    size_t textSize = ftell(myFile);
    fclose(myFile);
    return textSize;
}

int readFile(const char inPath[], char *text, size_t textSize) {
    assert(inPath != "");
    assert(text != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fread(text, 1, textSize, myFile);
    fclose(myFile);
    return 0;
}

int nRows(const char *str, size_t textSize, char element) {
    assert(str != nullptr);

    int nRows = 1;
    for (size_t i = 0; i < textSize; i++)
        if (str[i] == element) {
            nRows++;
        }
    return nRows;
}

void fillIndex(lineIndex *index, char *text, size_t textSize) {
    assert(index != nullptr);
    assert(text != nullptr);

    index[0].startIndex = text;
    int lines = 1;
    for (size_t i = 0; i < textSize; i++) {
        if (text[i] == '\n') {
            index[lines - 1].endIndex = &text[i] - 1;
            index[lines].startIndex = &text[i] + 1;

            lines++;

            text[i] = '\0';
        }
    }
    text[textSize] = '\0';
    index[lines - 1].endIndex = &text[textSize] - 1;
}

lineIndex *readTextFromFile(const char inPath[], char *text, size_t *textSize, size_t *rows) {
    assert(inPath != nullptr);

    *textSize = getFileSize(inPath);

    text = (char *) calloc(*textSize + 1, sizeof(char));

    if (readFile(inPath, text, *textSize)) exit(errno);
    *rows = nRows(text, *textSize, '\n');

    auto *index = (lineIndex *) calloc(*rows, sizeof(lineIndex));
    fillIndex(index, text, *textSize);

    return index;
}
