.MODEL SMALL 
.STACK 100H 

.DATA

rt	DW	?
a1	DW	?
b1	DW	?
t0	DW	?
t1	DW	?
t2	DW	?
t3	DW	?
t4	DW	?
t5	DW	?
t6	DW	?
t7	DW	?
c1	DW	3	DUP(?)

.CODE

PRINT PROC 
PUSHA 
MOV BX, AX 
CMP BX, 0 
JE CORNER 
JNL SKIP 
MOV AH, 2 
MOV DL, '-' 
INT 21H 
NEG BX 
SKIP: 
MOV CX, 0 
MOV AX, BX 
VAG: 
CMP AX, 0 
JE END_VAG 
MOV DX, 0 
MOV BX, 10 
IDIV BX 
PUSH DX 
INC CX 
JMP VAG 
END_VAG: 
OUTPUT: 
POP DX 
ADD DX, '0' 
MOV AH, 2 
INT 21H 
LOOP OUTPUT 
JMP DONE 
CORNER: 
MOV DX, '0' 
MOV AH, 2 
INT 21H 
DONE: 
MOV AH, 2 
MOV DL, ' ' ;0DH 
INT 21H 
;MOV DL, 0AH 
;INT 21H 
POPA 
RET 
PRINT ENDP 

MAIN PROC
MOV AX, @DATA
MOV DS, AX
MOV t0, 1	; new temp
MOV t1, 2	; new temp
MOV t2, 3	; new temp
MOV AX, t1
ADD AX, t2
MOV t3, AX	; new temp
MOV AX, t0
IMUL t3
MOV t4, AX	; new temp
MOV t5, 3	; new temp
MOV AX, t4
CMP AX, 0
MOV DX, 0
JNL @L0
MOV DX, -1
@L0:
IDIV t5
MOV t6, DX	; new temp
MOV AX, t6
MOV a1, AX
MOV t7, AX	; new temp
MOV t0, 1	; new temp
MOV t1, 5	; new temp
MOV AX, t0
CMP AX, t1
JL @L1
MOV t2, 0	; new temp
JMP @L2
@L1:
MOV t2, 1
@L2:
MOV AX, t2
MOV b1, AX
MOV t3, AX	; new temp
MOV t1, 2	; new temp
MOV t0, 0	; new temp
MOV BX, t0
ADD BX, BX
MOV AX, t1
MOV c1[BX], AX
MOV t2, AX	; new temp
MOV t1, 69	; new temp
MOV t0, 2	; new temp
MOV BX, t0
ADD BX, BX
MOV AX, t1
MOV c1[BX], AX
MOV t2, AX	; new temp
CMP a1, 0
JE @L3
CMP b1, 0
JE @L3
MOV t0, 1	; new temp
JMP @L4
@L3:
MOV t0, 0
@L4:
CMP t0, 0
JE @L5
MOV t1, 0	; new temp
MOV BX, t1
ADD BX, BX
MOV AX, c1[BX]
MOV t2, AX	; new temp
INC c1[BX]
JMP @L6
@L5:
MOV t1, 0	; new temp
MOV BX, t1
ADD BX, BX
MOV AX, c1[BX]
MOV t2, AX	; new temp
MOV t3, 2	; new temp
MOV BX, t3
ADD BX, BX
MOV AX, c1[BX]
MOV t4, AX	; new temp
MOV AX, t2
ADD AX, t4
MOV t5, AX	; new temp
MOV t0, 1	; new temp
MOV BX, t0
ADD BX, BX
MOV AX, t5
MOV c1[BX], AX
MOV t6, AX	; new temp
@L6:
MOV t0, 1	; new temp
MOV BX, t0
ADD BX, BX
MOV AX, c1[BX]
MOV t1, AX	; new temp
MOV AX, t1
MOV a1, AX
MOV t2, AX	; new temp
MOV AX, a1
CALL PRINT
MOV AX, b1
CALL PRINT
done_main:
MOV AH, 4CH 
INT 21H
MAIN ENDP 

END MAIN


