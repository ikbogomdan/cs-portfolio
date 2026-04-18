  .intel_syntax noprefix

  .text
  .global my_memcpy

my_memcpy: 
  push rbx
  test edx, edx 
  jz skip 
  
  again: 

    cmp edx, 8
    jl last

    mov rcx, [rsi]
    mov [rdi], rcx
    add rdi, 8
    add rsi, 8
    sub edx, 8 

    jmp again

    last:
    test edx, edx
    jz skip
    mov bl, [rsi]
    mov [rdi], bl
    add rdi, 1
    add rsi, 1
    dec edx 
    jmp last 

  skip: 
  pop rbx
  ret

