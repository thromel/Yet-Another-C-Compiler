.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
temp0 DW ?
a1_2 DW ?
b1_2 DW ?
c1_2 DW ?
maxab1_2 DW ?
maxabc1_2 DW ?
n1_3 DW ?
i1_3 DW ?
fact1_3 DW ?
temp1 DW ?
x1_4 DW ?
return_loc DW ?

.CODE
max2 PROC

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
JNE L3
 
MOV AX, a1_1

JMP L0
JMP L4
L3:
 
MOV AX, b1_1

JMP L0
L4:

L0: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
max2 ENDP


max3 PROC

POP return_loc
POP c1_2
POP b1_2
POP a1_2
PUSH BX
PUSH CX
PUSH DX

 
 
 
PUSH return_loc
PUSH a1_2
PUSH b1_2
CALL max2
POP return_loc
MOV AX, AX
MOV maxab1_2, AX 

 
PUSH return_loc
PUSH c1_2
PUSH maxab1_2
CALL max2
POP return_loc
MOV AX, AX

JMP L5
L5: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
max3 ENDP


factorial PROC

POP return_loc
POP n1_3
PUSH BX
PUSH CX
PUSH DX

 
 
 
MOV AX, 1
MOV i1_3, AX 

 
 
MOV AX, i1_3
MOV fact1_3, AX 

 
;for loop start
 
 
MOV AX, 1
MOV i1_3, AX 

L9:
 
 
MOV AX, i1_3
CMP AX, n1_3
JLE L7
MOV temp0, 0
JMP L8
L7: 
MOV temp0, 1
L8: 

MOV AX, temp0
CMP AX, 0
JE L10
 
 
 
 
MOV AX, fact1_3
MOV BX, i1_3
IMUL BX
MOV temp1, AX

MOV AX, temp1
MOV fact1_3, AX 

MOV AX, i1_3
MOV temp0, AX
INC AX
MOV i1_3, AX

JMP L9
L10:
;for loop end
 
MOV AX, fact1_3

JMP L6
L6: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
factorial ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
PUSH return_loc
PUSH 5
PUSH 3
PUSH 2
CALL max3
POP return_loc
MOV AX, AX
MOV x1_4, AX 

 
 
PUSH return_loc
PUSH x1_4
CALL factorial
POP return_loc
MOV AX, AX
MOV x1_4, AX 


MOV AX, x1_4
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
