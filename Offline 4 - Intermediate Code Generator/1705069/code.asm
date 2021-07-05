.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
t1_1 DW ?
temp0 DW ?
temp1 DW ?
temp2 DW ?
x1_2 DW ?
return_loc DW ?

.CODE
sumf PROC

POP return_loc
POP b1_1
POP a1_1
PUSH BX
PUSH DX

 
 
MOV AX, a1_1
CMP AX, 0
JE L1
MOV AX, 0
JMP L2
L1:
MOV AX, 1
L2:
MOV temp0, AX
MOV AX, temp0
CMP AX, 1
JNE L3
 
MOV CX, 0

JMP L0
L3:

 
 
MOV AX, a1_1
ADD AX, b1_1
MOV temp0, AX

 
 
 
MOV AX, a1_1
SUB AX, 1
MOV temp1, AX

 
 
MOV AX, b1_1
SUB AX, 1
MOV temp2, AX

PUSH a1_1
PUSH b1_1
PUSH t1_1
PUSH temp0
PUSH temp1
PUSH temp2
PUSH return_loc
PUSH temp1
PUSH temp2
CALL sumf
POP return_loc
POP temp2
POP temp1
POP temp0
POP t1_1
POP b1_1
POP a1_1
MOV AX, temp0
ADD AX, CX
MOV temp2, AX

MOV CX, temp2

JMP L0
L0: 
POP DX
POP BX
PUSH return_loc
RET
sumf ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
 
 
PUSH a1_1
PUSH b1_1
PUSH t1_1
PUSH temp0
PUSH temp1
PUSH temp2
PUSH return_loc
PUSH 10
PUSH 11
CALL sumf
POP return_loc
POP temp2
POP temp1
POP temp0
POP t1_1
POP b1_1
POP a1_1
MOV AX, CX
MOV x1_2, AX 


MOV AX, x1_2
CALL OUTDEC

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
