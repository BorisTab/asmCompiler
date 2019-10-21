//
// Created by boristab on 17.10.2019.
//

DEF_CMD(END, 0, 0, {
    endChecker = true;
    break;
})

DEF_CMD(PUSH, 1, 1, {
    stackPush(&cpuStk, *((int *)(++cur)));
    cur += sizeof(int);
    break;
})

DEF_CMD(ADD, 2, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) + stkPop(&cpuStk));
    cur++;
    break;
})

DEF_CMD(SUB, 3, 0, {
    int tmp = stkPop(&cpuStk);
    stackPush(&cpuStk, stkPop(&cpuStk) - tmp);
    cur++;
    break;
})

DEF_CMD(MUL, 4, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) * stkPop(&cpuStk));
    cur++;
    break;
})

DEF_CMD(DIV, 5, 0, {
    int tmp = stkPop(&cpuStk);
    stackPush(&cpuStk, stkPop(&cpuStk) / tmp);
    cur++;
    break;
})

DEF_CMD(POP, 6, 2, {
    registers[*cur - 'a'] = stkPop(&cpuStk);
    cur++;
    break;
})

DEF_CMD(JMP, 7, 1, {
    cur = buffer + *((int *)(++cur)) - 1;
    break;
})

DEF_CMD(PUSH, 11, 2, {
    cur++;
    stackPush(&cpuStk, registers[*cur - 'a']);
    cur++;
    break;
})

DEF_CMD(POP, 16, 2, {
    cur++;
    registers[*cur - 'a'] = stkPop(&cpuStk);
    cur++;
    break;
})

