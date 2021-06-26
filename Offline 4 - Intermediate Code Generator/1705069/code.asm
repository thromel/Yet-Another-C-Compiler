.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
temp0 DW ?
temp1 DW ?
temp2 DW ?
temp3 DW ?
return_loc DW ?
c1_1 DW 3 DUP (?)

.CODE
MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
 
 
 
 
MOV AX, 2
ADD AX, 3
MOV temp0, AX

MOV AX, 1
MOV BX, temp0
IMUL BX
MOV temp1, AX

 
MOV AX, temp1
MOV BX, 3
MOV AX, AX
CWD
IDIV BX
MOV temp2, DX

MOV AX, temp2
MOV a1_1, AX 

 
 
 
MOV AX, 1
CMP AX, 5
JL L0
MOV temp2, 0
JMP L1
L0: 
MOV temp2, 1
L1: 

MOV AX, temp2
MOV b1_1, AX 

MOV SI, 0

 
MOV AX, 2
MOV c1_1[SI], AX 

 
 
MOV AX, a1_1
CMP AX, 0
JE L2
MOV AX, b1_1
CMP AX, 0
JE L2
MOV AX, 1
JMP L3
L2:
MOV AX, 0
L3:
MOV temp2, AX
MOV AX, temp2
CMP AX, 1
JNE L4
MOV AX, c1_1[SI]
MOV temp3, AX
INC AX
MOV c1_1[SI], AX

JMP L5
L4:
MOV SI, 1

MOV SI, 0

MOV AX, c1_1[SI]
MOV c1_1[SI], AX 

L5:


MOV AX, a1_1
CALL OUTDEC


MOV AX, b1_1
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
