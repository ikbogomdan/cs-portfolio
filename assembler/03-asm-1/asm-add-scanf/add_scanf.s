.intel_syntax noprefix     
.text                      
.global add_scanf      
 
add_scanf: 

    sub rsp, 24
	mov rdi, offset format_string_scanf
	mov rsi, rsp 
	call scanf 
	mov rsi, rsp
	mov rdi, offset format_string_scanf
	add rsi, 8  
	call scanf
	mov rsi, rsp

	mov rax, [rsi] 
	add rsi, 8
	add rax, [rsi]
	add rsp, 24
	ret  

.data 
format_string_scanf:
	.string "%ld"  

