.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
c1_1 DW ?
a1_2 DW ?
b1_2 DW ?
c1_2 DW ?
temp0 DW ?
d1_1 DW 20 DUP (?)
d1_2 DW 20 DUP (?)

.CODE
 

 
 

MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
MOV AX, 20
MOV a1_2, AX 

 
MOV AX, 69
MOV b1_2, AX 

MOV AX, a1_2
ADD AX, 6
MOV temp0, AX

MOV AX, temp0
MOV c1_2, AX 

MOV AX, c1_2
ADD AX, a1_2
MOV temp0, AX

MOV AX, temp0
MOV b1_2, AX 

 
MOV AX, b1_2
SUB AX, c1_2
MOV temp0, AX

MOV AX, temp0
MOV a1_2, AX 

 
MOV AX, a1_2
INC AX
MOV a1_2, AX

 
MOV AX, b1_2
MOV temp0, AX
NEG temp0

MOV AX, temp0
MOV a1_2, AX 

 
MOV AX, a1_2
MOV BX, c1_2
IMUL BX
MOV temp0, AX

MOV AX, temp0
MOV b1_2, AX 

 
MOV AX, b1_2
MOV BX, 2
MOV AX, AX
CWD
IDIV BX
MOV temp0, AX

MOV AX, temp0
MOV b1_2, AX 

 
MOV AH, 4CH
INT 21H
MAIN ENDP



END MAIN
