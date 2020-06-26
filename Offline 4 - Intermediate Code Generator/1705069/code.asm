.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
temp0 DW ?
temp1 DW ?
a1_2 DW ?
b1_2 DW ?
x1_2 DW ?
temp2 DW ?
temp3 DW ?
a1_3 DW ?
b1_3 DW ?
temp4 DW ?
return_loc DW ?

.CODE
f PROC

POP return_loc
POP a1_1
PUSH BX
PUSH CX
PUSH DX

 
 
 
MOV AX, 2
MOV BX, a1_1
IMUL BX
MOV temp0, AX

MOV AX, temp0

MOV temp1, AX
JMP L0
 
 
MOV AX, 9
MOV a1_1, AX 

L0: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
f ENDP


g PROC

POP return_loc
POP b1_2
POP a1_2
PUSH BX
PUSH CX
PUSH DX

 
 
 
PUSH a1_2
CALL f
 
 
MOV AX, temp2
ADD AX, b1_2
MOV temp3, AX

MOV AX, temp3
MOV x1_2, AX 

 
MOV AX, x1_2

MOV temp3, AX
JMP L1
L1: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
g ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
MOV AX, 1
MOV a1_3, AX 

 
 
MOV AX, 2
MOV b1_3, AX 

 
 
PUSH a1_3
PUSH b1_3
CALL g
MOV AX, AX
MOV a1_3, AX 


MOV AX, a1_3
CALL OUTDEC

 
MOV AX, 0

MOV temp4, AX
JMP 
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
