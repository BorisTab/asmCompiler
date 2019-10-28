#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <cstdlib>
#include <cctype>
#include <cstring>

const int MaxComLen = 16;
const int MaxArgLen = 16;

enum errors {
    SUCCESS = 0,
    WRONG_FILE = 1,
    REGISTER_IS_NOT_EXIST = 2,
    SYNTAX_ERROR = 3,
};

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

void jump(int num, char **binBuffer);

int main(const int argc, char * const argv[]) {
    char *inPath = argv[argc - 1];
    char *pathPoint = strrchr(inPath, '.');

    if (strcmp(pathPoint, ".asm") != 0) {
        printf("Wrong file type");
        return WRONG_FILE;
    }

    char outPath[FILENAME_MAX] = "";
    strncpy(outPath, inPath, pathPoint - inPath);
    strcpy(outPath + (pathPoint - inPath), ".bin");

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
        }

#define DEF_CMD(name, num, argType, code, asmCode) \
            if (strcmp(str, #name) == 0) { \
                asmCode \
                parser(num, argType, &binBuffer, arg); \
            } else

#define DEF_JMP(num) \
            jump(num, &binBuffer); \
            continue;

            #include "commands.h"
        {}
#undef DEF_JMP
#undef DEF_CMD
    }

    binBuffer = bufferStart;

    for (size_t i = 0; i < lines; i++) {
        char str[MaxComLen] = "";
        char arg[MaxArgLen] = "";
        sscanf(index[i].startIndex, "%s%s", str, arg);

        char *tagPointer = strchr(str, ':');
        if (tagPointer != nullptr) continue;

        if (*str == 'J' || strcmp(str, "CALL") == 0) {
            binBuffer++;
            for (size_t tag = 0; tag < lines; tag++) {
                if (tagList[tag].tagName != nullptr) {
                    if (strncmp(arg, tagList[tag].tagName, strlen(arg)) == 0) {
                        *((int *)binBuffer) = tagList[tag].tagByte;
//                        printf("%s %ld\n",tagList[tag].tagName, tagList[tag].tagByte);
                    }
                }
            }
            binBuffer += sizeof(int);
        }

#define DEF_CMD(name, num, argType, code, asmCode) \
            if (*str == 'J' || strcmp(str, "CALL") == 0) { \
                continue; \
            } \
            else if (strcmp(str, #name) == 0) { \
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

//! first parser. It doesn't fill jumps
//!
//! \param[in] num command number
//! \param[in] argType type of argument after command
//! \param[out] binBuffer out binary buffer
//! \param[in] arg argument to write to buffer
void parser(int num, int argType, char **binBuffer, char *arg) {
    assert(binBuffer);
    assert(*binBuffer);
    assert(arg);

    if (argType == 0) {
        sprintf(*binBuffer, "%c", num);
        (*binBuffer)++;
    }
    else {
        if (isdigit(*arg) || (*arg == '-' && isdigit(*(arg + 1)))) {
            int digitArg = atoi(arg);
            sprintf(*binBuffer, "%c", num);
            (*binBuffer)++;

            *((int *) *binBuffer) = digitArg;
            *binBuffer += sizeof(int);
        }
        else if (isalpha(*arg)) {
            char *xPointer = strchr(arg, 'x');
            if (xPointer != nullptr && *(xPointer - 1) >= 'a' && *(xPointer - 1) <= 'd') {
                sprintf(*binBuffer, "%c", num + 50);
                (*binBuffer)++;
                sprintf(*binBuffer, "%c", *(xPointer - 1));
                (*binBuffer)++;
            }
            else {
                printf("Error: register is not exist\n");
                exit(REGISTER_IS_NOT_EXIST);
            }
        }
        else {
            printf("Error: Syntax error");
            exit(SYNTAX_ERROR);
        }
    }
}

//! second parser. Fill jumps
//!
//! \param[in] argType type of argument after command
//! \param[out] binBuffer out binary buffer
//! \param[in] arg argument to write to buffer
void secondParser(int argType, char **binBuffer, char *arg) {
    assert(binBuffer);
    assert(*binBuffer);
    assert(arg);

    if (argType == 0) {
        (*binBuffer)++;
    }
    else {
        if (isdigit(*arg) || (*arg == '-' && isdigit(*(arg + 1)))) {
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

//! Fill jumps with zero
//!
//! \param[in] num command number
//! \param[out] binBuffer out binary buffer
void jump(int num, char **binBuffer) {
    assert(binBuffer);
    assert(*binBuffer);

    **binBuffer = (char) num;
    (*binBuffer)++;
    *((int *) *binBuffer) = 0;
    *binBuffer += sizeof(int);
}
