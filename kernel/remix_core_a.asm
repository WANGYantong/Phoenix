
    INCLUDE remix_core_a_private.h

    PRESERVE8

    AREA    REMIX_CORE, CODE, READONLY
    THUMB

REMIX_CalcPrioTemp
    RBIT   R0, R0
    CLZ    R0, R0
    BX     LR

REMIX_PendSvContextSwitch

    PUSH   {R14}

    LDR    R0, =REMIX_TaskSched
    ADR.W  R14, {PC} + 0x7
    BX     R0

    MOV    R14, R13
    LDR    R0, =gpstrCurTaskReg
    LDR    R12, [R0]
    MOV    R0, R12
    CBZ    R0, __BACKUP_REG
    ADD    R14, #0x4
    LDMIA  R14!, {R0 - R3}
    STMIA  R12!, {R0 - R11}
    LDMIA  R14, {R0 - R3}
    SUB    R14, #0x10
    STMIA  R12!, {R0}
    STMIA  R12!, {R14}
    STMIA  R12!, {R1 - R3}
    POP    {R0}
    STMIA  R12, {R0}

__BACKUP_REG

    LDR    R0, =gpstrNextTaskReg
    LDR    R1, [R0]
    ADD    R1, #0x10
    LDMIA  R1!, {R4 - R11}
    ADD    R1, #0x4
    LDMIA  R1, {R13}
    ADD    R1, #0x10
    LDMIA  R1, {R0}
    BX     R0


REMIX_SwitchToTask

    LDR    R0, =gpstrNextTaskReg
    LDR    R13, [R0]

    ADD    R13, #0x40
    POP    {R0}
    MSR    XPSR, R0
    SUB    R13, #0x8
    LDMIA  R13, {R0}
    SUB    R13, #0x3C
    STMDB  R13, {R0}
    POP    {R0 - R12}
    ADD    R13, #0x4
    POP    {LR}
    SUB    R13, #0x40
    POP    {PC}


REMIX_TaskOccurSwi

    SVC    #0
    BX     R14


REMIX_GetXpsr

    MRS    R0, XPSR
    BX     R14


REMIX_SetBasepri

    MRS    R1, BASEPRI
    MSR    BASEPRI, R0
    MOV    R0, R1
    BX 	   R14


    ALIGN

    END


