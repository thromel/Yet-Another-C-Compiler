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
