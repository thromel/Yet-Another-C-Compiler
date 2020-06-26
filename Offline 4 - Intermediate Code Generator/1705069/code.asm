.MODEL MEDIUM 
.STACK 100H 
.DATA

x1_1 DW ?
temp0 DW ?
temp1 DW ?
a1_2 DW ?
b1_2 DW ?
return_loc DW ?

.CODE
factorial PROC

POP return_loc
POP x1_1
PUSH BX
PUSH CX
PUSH DX

 
 
MOV AX, x1_1
CMP AX, 1
JE L1
MOV temp0, 0
JMP L2
L1: 
MOV temp0, 1
L2: 

MOV AX, temp0
CMP AX, 1
JNE L5
 
MOV AX, x1_1

JMP L3
JMP L6
L5:
 
 
MOV AX, x1_1
ADD AX, 1
MOV temp1, AX

MOV AX, temp1

JMP L4
L6:

L4: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
factorial ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
MOV AX, 5
MOV a1_2, AX 

 
 
PUSH return_loc
PUSH a1_2
CALL factorial
POP return_loc
MOV AX, AX
MOV b1_2, AX 


MOV AX, b1_2
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
