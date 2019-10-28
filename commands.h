//
// Created by boristab on 17.10.2019.
//

DEF_CMD(END, 0, 0, {
    endChecker = true;
    break;
}, {})

DEF_CMD(PUSH, 1, 1, {
    stackPush(&cpuStk, *((int *)(++cur)));
    cur += sizeof(int);
    break;
}, {})

DEF_CMD(ADD, 2, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) + stkPop(&cpuStk));
    cur++;
    break;
}, {})

DEF_CMD(SUB, 3, 0, {
    int tmp = stkPop(&cpuStk);
    stackPush(&cpuStk, stkPop(&cpuStk) - tmp);
    cur++;
    break;
}, {})

DEF_CMD(MUL, 4, 0, {
    stackPush(&cpuStk, stkPop(&cpuStk) * stkPop(&cpuStk));
    cur++;
    break;
}, {})

DEF_CMD(DIV, 5, 0, {
    int tmp = stkPop(&cpuStk);
    stackPush(&cpuStk, stkPop(&cpuStk) / tmp);
    cur++;
    break;
}, {})

DEF_CMD(POP, 6, 2, {
    registers[*cur - 'a'] = stkPop(&cpuStk);
    cur++;
    break;
}, {})

DEF_CMD(JMP, 7, 1, {
    cur = buffer + *((int *)(++cur));
    break;
}, DEF_JMP(7))

DEF_CMD(JA, 8, 1, {
    DEF_CJ(>)
    break;
}, DEF_JMP(8))

DEF_CMD(JAE, 9, 1, {
    DEF_CJ(>=)
    break;
}, DEF_JMP(9))

DEF_CMD(JB, 10, 1, {
    DEF_CJ(<)
    break;
}, DEF_JMP(10))

DEF_CMD(JBE, 11, 1, {
    DEF_CJ(<=)
    break;
}, DEF_JMP(11))

DEF_CMD(JE, 12, 1, {
    DEF_CJ(==)
    break;
}, DEF_JMP(12))

DEF_CMD(JNE, 13, 1, {
    DEF_CJ(!=)
    break;
}, DEF_JMP(13))

DEF_CMD(CALL, 14, 1, {
    stackPush(&callStk, cur + sizeof(char) + sizeof(int) - buffer);
    cur = buffer + *((int *)(++cur));
    break;
}, DEF_JMP(14))

DEF_CMD(RET, 15, 0, {
    if (stackSize(&callStk) != 0) cur = buffer + stkPop(&callStk);
    else cur++;
    break;
}, {})

DEF_CMD(OUT, 16, 0, {
    printf("%d\n", stkPop(&cpuStk));
    cur++;
    break;
}, {})

DEF_CMD(IN, 17, 0, {
    int val = 0;
    scanf("%d", &val);
    stackPush(&cpuStk, val);
    cur++;
    break;
}, {})

DEF_CMD(SQRT, 18, 0, {
    stackPush(&cpuStk, sqrt(stkPop(&cpuStk)));
    cur++;
    break;
}, {})

DEF_CMD(PUSH, 51, 2, {
    cur++;
    stackPush(&cpuStk, registers[*cur - 'a']);
    cur++;
    break;
}, {})

DEF_CMD(POP, 56, 2, {
    cur++;
    registers[*cur - 'a'] = stkPop(&cpuStk);
    cur++;
    break;
}, {})

