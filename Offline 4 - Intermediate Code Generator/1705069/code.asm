.MODEL MEDIUM 
.STACK 100H 
.DATA

n1_1 DW ?
i1_1 DW ?
fact1_1 DW ?
temp0 DW ?
temp1 DW ?
return_loc DW ?

.CODE
fact PROC

POP return_loc
POP n1_1
PUSH BX
PUSH CX
PUSH DX

 
 
 
MOV AX, 1
MOV fact1_1, AX 

 
 
MOV AX, fact1_1
MOV i1_1, AX 

 
;for loop start
 
 
MOV AX, 1
MOV i1_1, AX 

L3:
 
 
MOV AX, i1_1
CMP AX, n1_1
JLE L1
MOV temp0, 0
JMP L2
L1: 
MOV temp0, 1
L2: 

MOV AX, temp0
CMP AX, 0
JE L4
 
 
 
 
MOV AX, fact1_1
MOV BX, i1_1
IMUL BX
MOV temp1, AX

MOV AX, temp1
MOV fact1_1, AX 


MOV AX, fact1_1
CALL OUTDEC

MOV AX, i1_1
MOV temp0, AX
INC AX
MOV i1_1, AX

JMP L3
L4:
;for loop end
L0: 
POP DX
POP CX
POP BX
PUSH return_loc
RET
fact ENDP


MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
PUSH return_loc
PUSH 10
CALL fact
POP return_loc
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
