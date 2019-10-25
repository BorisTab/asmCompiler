#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <map>

const int MaxComLen = 16;
const int MaxArgLen = 16;

struct lineIndex {
    char *startIndex;
    char *endIndex;
};

struct tag {
    char *tagName;
    size_t tagByte;
};

int getFileSize(const char *inPath);

int readFile(const char inPath[], char *text, size_t textSize);

int nRows(const char *str, size_t textSize, char element);

void fillIndex(lineIndex *index, char *text, size_t textSize);

lineIndex *readTextFromFile(const char inPath[], char *text, size_t *textSize, size_t *rows);

void parser(int num, int argType, char **binBuffer, char *arg);

void secondParser(int argType, char **binBuffer, char *arg);

int main() {
    char inPath[FILENAME_MAX] = "../program-v1.asm";
    char outPath[FILENAME_MAX] = "../program-v1.bin";

    FILE *binFile = fopen(outPath, "wb");

    size_t programSize = 0;
    size_t lines = 0;
    char *buffer = nullptr;

    lineIndex *index = readTextFromFile(inPath, buffer, &programSize, &lines);

    char *binBuffer = (char *) calloc(lines * (MaxComLen + MaxArgLen + 3), sizeof(char));
    char *bufferStart = binBuffer;

    tag *tagList = (tag *) calloc(lines, sizeof(tag));

    for (size_t i = 0; i < lines; i++) {
        char str[MaxComLen] = "";
        char arg[MaxArgLen] = "";
        sscanf(index[i].startIndex, "%s%s", str, arg);

        char *tagPointer = strchr(str, ':');
        if (tagPointer != nullptr) {
            tagList[i].tagName = index[i].startIndex;
            tagList[i].tagByte = (size_t) (binBuffer - bufferStart);

            char *b = tagList[i].tagName;
        }
        if (strcmp(str, "JMP") == 0) {
            *binBuffer = 7;
            binBuffer++;
            *(int *) binBuffer = 0;
            binBuffer += sizeof(int);
        }

#define DEF_CMD(name, num, argType, code) \
            if (strcmp(str, #name) == 0) { \
                parser(num, argType, &binBuffer, arg); \
            } else

            #include "commands.h"
        {}
#undef DEF_CMD
    }

    binBuffer = bufferStart;

    for (size_t i = 0; i < lines; i++) {
        char str[MaxComLen] = "";
        char arg[MaxArgLen] = "";
        sscanf(index[i].startIndex, "%s%s", str, arg);

        char *tagPointer = strchr(str, ':');
        if (tagPointer != nullptr) continue;

        if (strcmp(str, "JMP") == 0) {
            binBuffer++;
            for (size_t tag = 0; tag < lines; tag++) {
                if (tagList[tag].tagName != nullptr) {
                    char *a = tagList[tag].tagName;
                    if (strncmp(arg, tagList[tag].tagName, strlen(arg)) == 0) {
                        *((int *)binBuffer) = tagList[tag].tagByte;
                    }
                }
            }
            binBuffer += sizeof(int);
        }

#define DEF_CMD(name, num, argType, code) \
            if (strcmp(str, #name) == 0) { \
                secondParser(argType, &binBuffer, arg); \
            } else

#include "commands.h"
        {}
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

void parser(int num, int argType, char **binBuffer, char *arg) {
    assert(binBuffer);
    assert(*binBuffer);
    assert(arg);

    if (argType == 0) {
        sprintf(*binBuffer, "%c", num);
        (*binBuffer)++;
    }
    else {
        if (int digitArg = atoi(arg)) {
            sprintf(*binBuffer, "%c", num);
            (*binBuffer)++;

            *((int *) *binBuffer) = digitArg;
            *binBuffer += sizeof(int);
        }
        else if (isalpha(*arg)) {
            char *xPointer = strchr(arg, 'x');
            if (xPointer != nullptr) {
                sprintf(*binBuffer, "%c", num + 10);
                (*binBuffer)++;
                sprintf(*binBuffer, "%c", *(xPointer - 1));
                (*binBuffer)++;
            }
        }
    }
}

void secondParser(int argType, char **binBuffer, char *arg) {
    if (argType == 0) {
        (*binBuffer)++;
    }
    else {
        if (int digitArg = atoi(arg)) {
            (*binBuffer)++;
            (*binBuffer) += sizeof(int);
        }
        else if (isalpha(*arg)) {
            char *xPointer = strchr(arg, 'x');

            if (*xPointer == 'x') {
                (*binBuffer) += 2;
            }
        }
    }
}
