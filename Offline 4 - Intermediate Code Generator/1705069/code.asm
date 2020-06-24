.MODEL MEDIUM 
.STACK 100H 
.DATA

x1_1 DW ?
y1_1 DW ?
min1_1 DW ?
temp0 DW ?

.CODE
MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
MOV AX, -1
MOV min1_1, AX 

 
MOV AX, 20
MOV x1_1, AX 

 
MOV AX, 30
MOV y1_1, AX 

 
 
MOV AX, y1_1
CMP AX, x1_1
JL L0
MOV temp0, 0
JMP L1
L0: 
MOV temp0, 1
L1: 

MOV AX, temp0
CMP AX, 1
JNE L2
 
MOV AX, x1_1
MOV min1_1, AX 

L2:


MOV AX, min1_1
CALL OUTDEC

MOV AH, 4CH
INT 21H
MAIN ENDP


 OUTDEC PROC
    ;Outputs a signed decimal integer
    ;INPUT: AX
    ;OUTPUT: NONE
    
    
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH AX
    
    ;if AX < 0
    CMP AX, 0
    JGE @END_IF ;if AX >= 0
    ;then
    PUSH AX ;save AX
    MOV AH, 2
    MOV DL, '-'
    INT 21H
    
    POP AX ;get the number back from stack
    NEG AX ; AX = -AX 
    
    ;else
    @END_IF:
        MOV CX, 0 ;CX counts digits
        MOV BX, 10
    
    @WHILE1:
        MOV DX, 0 ; Dividend
        DIV BX ; Quo:Rem = AX:DX
        PUSH DX; Save rem on stack
        INC CX;
        
        CMP AX, 0 ; quo = 0?
        JNE @WHILE1
        
    MOV AH, 2
        
    @PRINT_LOOP:    
        POP DX
        ADD DL, 30H
        INT 21H
        LOOP @PRINT_LOOP
        
    ;exit
   
    ;Linebreak
    MOV AH, 2
    MOV DL, 0DH
    INT 21H
    MOV DL, 0AH
    INT 21H
    
    
    POP AX
    POP DX
    POP CX
    POP BX
    RET   
    
OUTDEC ENDP


END MAIN
