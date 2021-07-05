.MODEL MEDIUM 
.STACK 100H 
.DATA

x1_1 DW ?
temp0 DW ?
temp1 DW ?
return_loc DW ?

.CODE
rec PROC

POP return_loc
POP x1_1
PUSH BX
PUSH CX
PUSH DX

MOV AX, x1_1
CMP AX, 0
JG L1
MOV temp0, 0
JMP L2
L1: 
MOV temp0, 1
L2: 

MOV AX, temp0
CMP AX, 1
JNE L3
PUSH x1_1
MOV AX, x1_1
SUB AX, 1
MOV temp1, AX

PUSH return_loc
PUSH temp1
CALL rec
POP return_loc
POP x1_1
L3:


MOV AX, x1_1
CALL OUTDEC

L0: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
rec ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

PUSH x1_1
PUSH return_loc
PUSH 5
CALL rec
POP return_loc
POP x1_1
MOV AH, 4CH
INT 21H
MAIN ENDP


OUTDEC PROC
PUSH BX
PUSH CX
PUSH DX
PUSH AX
CMP AX, 0
JGE @END_IF
PUSH AX ;save AX
MOV AH, 2
MOV DL, '-'
INT 21H
    
POP AX ;get the number back from stack
NEG AX ; AX = -AX 
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
