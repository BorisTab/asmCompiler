//
// Created by BorisTab on 01.10.2019.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H
//#define DEBUG

/*!
	@file
    @brief My stack
*/

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

typedef int stkElem_t;

const int reallocDelta = 2;
const int hashSeed = 100;

const stkElem_t dataCanary1 = 1523234663;
const stkElem_t dataCanary2 = 1675494968;

const size_t stackCanary1 = 155859430405;
const size_t stackCanary2 = 554930349559;

enum err {
    SUCCESS = 0,
    STACK_CONSTRUCT_FAILED = 1,
    STACK_REALLOC_FAILED = 2,
    STACK_EMPTY = 3,
};

struct Stack_t {
    size_t stackCanary1;
    stkElem_t *data;
    size_t maxDataSize;
    size_t size;
    int err;
    char * name;
    __uint32_t hash;
    size_t stackCanary2;
};

__uint32_t hash(const __uint8_t* key, size_t len, __uint32_t seed) {
    __uint32_t h = seed;

    if (len > 3) {
        size_t i = len >> 2;
        do {
            __uint32_t k;
            memcpy(&k, key, sizeof(__uint32_t));
            key += sizeof(__uint32_t);
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
    }

    if (len & 3) {
        size_t i = len & 3;
        __uint32_t k = 0;
        do {
            k <<= 8;
            k |= key[i - 1];
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }

    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

//!
//! \param stk[in] Pointer on stack
//! \param file[in] file name, where stack have been used (fill by default)
//! \param line[in] line, where stack have been used (fill by default)
void stackDump(Stack_t *stk, char *file = __FILE__, size_t line = __LINE__) {
    assert(stk);
    assert(file);

    printf ("Looking from %s line %lu\n", file, line);
    printf("Stack_t \"%s\" [%p] ",stk->name, stk);
    if (stk->err == 0) {
        printf("(Ok)\n");
    } else {
        printf("(Error)\n");
    }
    printf("{\n"
           "    size = %lu\n", stk->size);
    printf("    data[%lu] = [%p]\n", stk->maxDataSize, &(stk->data));
    printf("    {\n");
    printf("        [%d] = %d Canary\n", 0, stk->data[0]);

    for (size_t i = 1; i < stk->size + 1; i++) {
        printf("       *[%ld] = %d\n", i, stk->data[i]);
    }

    for (size_t i = stk->size + 1; i < stk->maxDataSize - 1; i++) {
        printf("        [%ld] = %d\n", i, stk->data[i]);
    }
    printf("        [%ld] = %d Canary\n", stk->maxDataSize - 1, stk->data[stk->maxDataSize]);

    printf("    }\n");
    printf("    err = %d\n", stk->err);

    printf("}\n\n");
}

//!
//! \param stk[in] stack for check
//! \return true if stack Ok, false if stack have errors
bool stackOK(Stack_t *stk) {
    assert(stk);

    if (hash((const __uint8_t *) stk->data, stk->maxDataSize, hashSeed) != stk->hash) {
        stackDump(stk);
        return false;
    }

    if (stk->err != 0 || stk->data[0] != dataCanary1 || stk->data[stk->maxDataSize] != dataCanary2) {
        stackDump(stk);
        return false;
    }

    if (stk->stackCanary1 != stackCanary1 || stk->stackCanary2 != stackCanary2) {
        stackDump(stk);
        return false;
    }

    return true;
}

//! stack constructor
//!
//! \param stk[in] Pointer to stack for construct
//! \return  1 if construct failed, 0 on success
int stackConstruct(Stack_t *stk) {
    assert(stk);

    stk->stackCanary1 = stackCanary1;
    stk->stackCanary2 = stackCanary2;

    stk->maxDataSize = 4;
    stk->data = (stkElem_t *) calloc(stk->maxDataSize, sizeof(stkElem_t));
    stk->size = 0;
    stk->err = 0;
    stk->name = "defName";
    stk->data[0] = dataCanary1;
    stk->data[stk->maxDataSize] = dataCanary2;
    stk->hash = hash((const __uint8_t *) stk->data, stk->maxDataSize, hashSeed);

    if (stk->data == nullptr) {
        stk->err = STACK_CONSTRUCT_FAILED;
#ifdef DEBUG
        assert(stk->data);
        assert(stackOK(stk));
#endif
        return STACK_CONSTRUCT_FAILED;
    }

    return SUCCESS;
}

//! change max size for stack
//!
//! \param stk[in] Pointer to stack for change its size
//! \return 2 on failed, 0 on success
Stack_t *changeStackSize(Stack_t *stk) {
#ifdef DEBUG
    assert(stackOK(stk));
#endif
    assert(stk);

    auto *tmp = (stkElem_t *) realloc(stk->data, reallocDelta * sizeof(stkElem_t *) * stk->maxDataSize);
    if (tmp == nullptr) return (Stack_t *) STACK_REALLOC_FAILED;

    stk->data = tmp;
    stk->data[stk->maxDataSize] = {};

    stk->maxDataSize *= reallocDelta;
    stk->data[stk->maxDataSize] = dataCanary2;

#ifdef DEBUG
    assert(stackOK(stk));
#endif
    return stk;
}

//!
//! \param stk[in] Pointer on stack to push
//! \param value[in] value to push
//! \return 0 on success
int stackPush(Stack_t *stk, stkElem_t value) {
#ifdef DEBUG
    assert(stackOK(stk));
#endif
    assert(stk);

    if (stk->size + 2 >= stk->maxDataSize) stk = changeStackSize(stk);
    stk->data[++stk->size] = value;

    stk->hash = hash((const __uint8_t *) stk->data, stk->maxDataSize, hashSeed);
#ifdef DEBUG
    assert(stackOK(stk));
#endif
    return SUCCESS;
}

//!
//! \param stk[in] Pointer to stack
//! \param var[out] Pointer on stkElem_T variable for pop
//! \return 3 on failed, 0 on success
int  stackPop( Stack_t *stk, stkElem_t *var) {
    assert(stk);
    assert(var);
#ifdef DEBUG
    assert(stackOK(stk));
#endif

    if(stk->size) {
        stk->size--;
        *var = stk->data[stk->size + 1];
        stk->data[stk->size + 1] = {};
        return SUCCESS;
    }

    stk->err = STACK_EMPTY;

    stk->hash = hash((const __uint8_t *) stk->data, stk->maxDataSize, hashSeed);
#ifdef DEBUG
    assert(stackOK(stk));
#endif
    return STACK_EMPTY;
}

//!
//! \param stk[in] stack to destruct
void stackDestruct(Stack_t *stk) {
    free(stk->data);
    stk->size = 0;
    stk->maxDataSize = 0;
}

#endif STACK_STACK_H
