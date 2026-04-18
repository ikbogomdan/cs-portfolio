.text
.global longest_inc_subseq
.align 2 

empty: // {
    mov x0, #0
    ret
// }
one_element: // {
    mov x0, #1
    ret
// }

update_value: // {
    mov x3, x23
    add x3, x3, #1
    str x3, [x1, x20, LSL #3]  // dp[i] = x3 
    B update_value_back
// }

max_value: // {
    LDR x23, [x1, x19, LSL #3] // dp[j]
    LDR x24, [x1, x20, LSL #3] // dp[i] 
    CMP x23, x24
    B.GE update_value 
    update_value_back:
    B max_value_back
// }

update_dp: // {
    mov x3, x21
    B update_dp_back
// }

longest_inc_subseq: // {
    cbz x2, empty  
    CMP x2, #1
    beq one_element  
    mov x3, #1
    str x3, [x1]  // dp[0] = 1 
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]! 
    mov x20, #1 
    // x21 - array[i] || x22 - array[j] ||  x19 - j from 0 to i - 1 || x20 - i 
    while_i: // {
        CMP x20, x2
        B.EQ while_i_break
        mov x19, #0 
        LDR x21, [x0, x20, LSL #3]  // x21 = array[i] 
        mov x3, #1
        str x3, [x1, x20, LSL #3]  // dp[i] = 1  
        while_j: // {
            CMP x19, x20
            B.EQ while_j_break
            LDR x22, [x0, x19, LSL #3]
            CMP x22, x21
            B.LT max_value
            max_value_back:
            ADD x19, x19, #1
            B while_j
        // }
        while_j_break: 
        ADD x20, x20, #1
        B while_i
    // }
    while_i_break: 
    
    mov x20, #0
    mov x3, #0
    while_k: // {
        CMP x20, x2
        B.EQ while_k_break 
        LDR x21, [x1, x20, LSL #3]
        CMP x21, x3
        B.GT update_dp
        update_dp_back: 
        ADD x20, x20, #1
        B while_k
    // }
    while_k_break:

    mov x0, x3
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
    ret 
// }

