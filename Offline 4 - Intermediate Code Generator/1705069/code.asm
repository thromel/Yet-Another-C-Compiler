.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
temp0 DW ?
x1_2 DW ?
y1_2 DW ?
max1_2 DW ?
return_loc DW ?

.CODE
maxnum PROC

POP return_loc
POP b1_1
POP a1_1
PUSH BX
PUSH CX
PUSH DX

 
 
MOV AX, a1_1
CMP AX, b1_1
JGE L1
MOV temp0, 0
JMP L2
L1: 
MOV temp0, 1
L2: 

MOV AX, temp0
CMP AX, 1
JNE L5
 
MOV AX, a1_1

JMP L3
JMP L6
L5:
 
MOV AX, b1_1

JMP L4
L6:

L4: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
maxnum ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
MOV AX, 10
MOV x1_2, AX 

 
 
MOV AX, 20
MOV y1_2, AX 

 
 
PUSH return_loc
PUSH x1_2
PUSH y1_2
CALL maxnum
POP return_loc
MOV AX, AX
MOV max1_2, AX 


MOV AX, max1_2
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
