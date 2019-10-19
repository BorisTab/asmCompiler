//
// Created by boristab on 17.10.2019.
//


#define DEF_CMD(name, num, argType, code) \
    CMD_##name = num,

enum commands {
#include "commands.h"
};

#undef DEF_CMD
