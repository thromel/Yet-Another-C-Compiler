.MODEL MEDIUM 
.STACK 100H 
.DATA

i1_1 DW ?
b1_1 DW ?
temp0 DW ?
temp1 DW ?
return_loc DW ?
a1_1 DW 20 DUP (?)

.CODE
MAIN PROC 

MOV AX,@DATA
MOV DS,AX

;for loop start
MOV AX, 0
MOV i1_1, AX 

L2:
MOV AX, i1_1
CMP AX, 20
JL L0
MOV temp0, 0
JMP L1
L0: 
MOV temp0, 1
L1: 

MOV AX, temp0
CMP AX, 0
JE L3
MOV AX, i1_1
MOV BX, 2
IMUL BX
MOV SI, AX
MOV AX, i1_1
ADD AX, 1
MOV temp1, AX

MOV a1_1[SI], AX 

MOV AX, i1_1
MOV temp0, AX
INC AX
MOV i1_1, AX

JMP L2
L3:
;for loop end
;for loop start
MOV AX, 0
MOV i1_1, AX 

L6:
MOV AX, i1_1
CMP AX, 20
JL L4
MOV temp1, 0
JMP L5
L4: 
MOV temp1, 1
L5: 

MOV AX, temp1
CMP AX, 0
JE L7
MOV AX, i1_1
MOV BX, 2
IMUL BX
MOV SI, AX
MOV AX, a1_1[SI]
MOV b1_1, AX 


CALL OUTDEC

MOV AX, i1_1
MOV temp1, AX
INC AX
MOV i1_1, AX

JMP L6
L7:
;for loop end
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
