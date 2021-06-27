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
n1_4 DW ?
term11_4 DW ?
term21_4 DW ?
ans1_4 DW ?
i1_4 DW ?
temp2 DW ?
i1_5 DW ?
max1_5 DW ?
temp3 DW ?
temp4 DW ?
temp5 DW ?
temp6 DW ?
return_loc DW ?
x1_5 DW 20 DUP (?)

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


fibonacci PROC

POP return_loc
POP n1_4
PUSH BX
PUSH CX
PUSH DX

 
 
 
MOV AX, 0
MOV term11_4, AX 

 
 
MOV AX, 1
MOV term21_4, AX 

 
 
MOV AX, 1
MOV ans1_4, AX 

 
;for loop start
 
 
MOV AX, 0
MOV i1_4, AX 

L14:
 
 
MOV AX, i1_4
CMP AX, n1_4
JL L12
MOV temp1, 0
JMP L13
L12: 
MOV temp1, 1
L13: 

MOV AX, temp1
CMP AX, 0
JE L15
 
 
MOV AX, term21_4
MOV term11_4, AX 

 
 
MOV AX, ans1_4
MOV term21_4, AX 

 
 
 
MOV AX, term11_4
ADD AX, term21_4
MOV temp2, AX

MOV AX, temp2
MOV ans1_4, AX 


MOV AX, ans1_4
CALL OUTDEC

MOV AX, i1_4
MOV temp1, AX
INC AX
MOV i1_4, AX

JMP L14
L15:
;for loop end
 
MOV AX, ans1_4

JMP L11
L11: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
fibonacci ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
MOV AX, 0
MOV max1_5, AX 

 
 
MOV AX, 6
MOV i1_5, AX 

 
 
 
 
MOV AX, 23
MOV BX, 6
MOV AX, AX
CWD
IDIV BX
MOV temp2, DX

PUSH return_loc
PUSH temp2
CALL factorial
POP return_loc
MOV AX, AX
MOV i1_5, AX 


MOV AX, i1_5
CALL OUTDEC

 
;for loop start
 
 
MOV AX, 0
MOV i1_5, AX 

L18:
 
 
MOV AX, i1_5
CMP AX, 20
JL L16
MOV temp2, 0
JMP L17
L16: 
MOV temp2, 1
L17: 

MOV AX, temp2
CMP AX, 0
JE L19
MOV SI, i1_5

 
 
 
MOV AX, i1_5
ADD AX, 1
MOV temp3, AX

 
 
MOV AX, i1_5
ADD AX, 1
MOV temp4, AX

MOV AX, temp3
MOV BX, temp4
IMUL BX
MOV temp5, AX

MOV AX, temp5
MOV x1_5[SI], AX 

MOV AX, i1_5
MOV temp2, AX
INC AX
MOV i1_5, AX

JMP L18
L19:
;for loop end
 
;for loop start
 
 
MOV AX, 0
MOV i1_5, AX 

L22:
 
 
MOV AX, i1_5
CMP AX, 20
JL L20
MOV temp5, 0
JMP L21
L20: 
MOV temp5, 1
L21: 

MOV AX, temp5
CMP AX, 0
JE L23
 
 
PUSH return_loc
PUSH x1_5[SI]
PUSH max1_5
CALL max2
POP return_loc
MOV AX, AX
MOV max1_5, AX 

MOV AX, i1_5
MOV temp5, AX
INC AX
MOV i1_5, AX

JMP L22
L23:
;for loop end
 
 
 
 
MOV AX, max1_5
MOV BX, 20
MOV AX, AX
CWD
IDIV BX
MOV temp6, AX

MOV AX, temp6
MOV max1_5, AX 

 
 
PUSH return_loc
PUSH 10
CALL fibonacci
POP return_loc
MOV AX, AX
MOV max1_5, AX 


MOV AX, max1_5
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
