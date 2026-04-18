    .intel_syntax noprefix
    .text
    .global dot_product

dot_product:
    push rbp

    vxorps ymm0, ymm0, ymm0
    pxor xmm1, xmm1 
    pxor xmm2, xmm2

    again:
        cmp rdi, 8
        jl last  
        
        vmovups ymm3, [rsi]   
        vmovups ymm4, [rdx] 

        vmulps ymm3, ymm3, ymm4
        vaddps ymm0, ymm0, ymm3 

        sub rdi, 8
        add rsi, 32
        add rdx, 32 
        jmp again 

    last:
        test rdi, rdi
        jz bb 
        vmovss xmm3, [rsi]
        vmovss xmm4, [rdx]
        vmulss xmm3, xmm3, xmm4
        vaddss xmm2, xmm2, xmm3
        add rsi, 4
        add rdx, 4
        dec rdi
        jmp last

    bb:
        vextractf128 xmm3, ymm0, 1
        vaddps xmm0, xmm0, xmm3
        vhaddps xmm0, xmm0, xmm0
        vhaddps xmm0, xmm0, xmm0
        vaddss xmm0, xmm0, xmm2
        pop rbp
        ret

