.MODEL MEDIUM 
.STACK 100H 
.DATA

a1_1 DW ?
b1_1 DW ?
c1_1 DW ?
i1_1 DW ?
temp0 DW ?
temp1 DW ?
return_loc DW ?

.CODE
MAIN PROC 

MOV AX,@DATA
MOV DS,AX

 
 
 
MOV AX, 0
MOV b1_1, AX 

 
 
MOV AX, 1
MOV c1_1, AX 

 
;for loop start
 
 
MOV AX, 0
MOV i1_1, AX 

L4:
 
 
MOV AX, i1_1
CMP AX, 4
JL L0
MOV temp0, 0
JMP L1
L0: 
MOV temp0, 1
L1: 

MOV AX, temp0
CMP AX, 0
JE L5
 
 
MOV AX, 3
MOV a1_1, AX 

 
;while loop start
L2:
MOV AX, a1_1
DEC AX
MOV a1_1, AX

MOV AX, a1_1
CMP AX, 0
JE L3
MOV AX, b1_1
INC AX
MOV b1_1, AX

JMP L2
L3:
;while loop end
MOV AX, i1_1
INC AX
MOV i1_1, AX

JMP L4
L5:
;for loop end

MOV AX, a1_1
CALL OUTDEC


MOV AX, b1_1
CALL OUTDEC


MOV AX, c1_1
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
