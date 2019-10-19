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
    stackPush(&cpuStk, stkPop(&cpuStk) - stkPop(&cpuStk));
    cur++;
    break;
})

DEF_CMD(MUL, 4, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) * stkPop(&cpuStk));
    cur++;
    break;
})

DEF_CMD(DIV, 5, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) / stkPop(&cpuStk));
    cur++;
    break;
})

