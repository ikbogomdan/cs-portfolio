.text
.global add_scanf

.macro push Xn
    sub sp, sp, 16
    str \Xn, [sp]
.endm

.macro pop Xn
    ldr \Xn, [sp]
    add sp, sp, 16
.endm

add_scanf: 
    push x30
    push x29
    mov x29, sp 
    sub sp, sp, #16 
    adrp x0, scanf_format_string
    add x0, x0, :lo12:scanf_format_string
    mov x1, sp                     
    bl scanf 
    adrp x0, scanf_format_string
    add x0, x0, :lo12:scanf_format_string
    add x1, sp, #8                
    bl scanf 
    ldr x0, [sp]                   
    ldr x1, [sp, #8]                
    add x0, x0, x1                 
    add sp, sp, #16            
    pop x29
    pop x30
    ret

.section .rodata
scanf_format_string:
    .string "%lld"                  
