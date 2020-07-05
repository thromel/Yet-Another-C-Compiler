.MODEL small 
.STACK 100h 
.DATA
	print_var dw ?
	ret_temp dw ?
	a_1 dw ?
	b_1 dw ?
	c_1 dw ?
	i_1 dw ?
	t0 dw ?
	t1 dw ?
	t2 dw ?
	t3 dw ?
.CODE
print PROC
	push ax
	push bx 
	push cx
	push dx
	mov ax, print_var
	mov bx, 10
	mov cx, 0
printLabel1:
	mov dx, 0
	div bx
	push dx
	inc cx
	cmp ax, 0
	jne printLabel1
printLabel2:
	mov ah, 2
	pop dx
	add dl, '0'
	int 21h
	dec cx
	cmp cx, 0
	jne printLabel2
	mov dl, 0Ah
	int 21h
	mov dl, 0Dh
	int 21h
	pop dx
	pop cx
	pop bx
	pop ax
	ret
print endp
main_proc PROC
	mov ax, @data
	mov ds, ax
	mov ax, 0
	mov b_1, ax
	mov ax, 1
	mov c_1, ax
	mov ax, 0
	mov i_1, ax
L4:
	mov ax, i_1
	cmp ax, 4
	jge L0
	mov t0, 1
	jmp L1
L0:
	mov t0, 0
L1:
	mov ax, t0
	cmp ax, 0
	je L5
	mov ax, 3
	mov a_1, ax
L2:
	mov ax, a_1
	mov t2, ax
	dec a_1
	mov ax, t2
	cmp ax, 0
	je L3
	mov ax, b_1
	mov t3, ax
	inc b_1
	jmp L2
L3:
	mov ax, i_1
	mov t1, ax
	inc i_1
	jmp L4
L5:
	mov ax, a_1
	mov print_var, ax
	call print
	mov ax, b_1
	mov print_var, ax
	call print
	mov ax, c_1
	mov print_var, ax
	call print
main_proc ENDP
END main_proc
