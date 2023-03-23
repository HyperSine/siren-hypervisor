.CODE

EXTERN ?dispatch@mshv_vmexit_handler@vmx@siren@@CA_NPEAVmshv_virtual_cpu@23@PEAUguest_state_t@23@@Z: PROC

?entry_point@mshv_vmexit_handler@vmx@siren@@SAXXZ PROC
    sub rsp, 100h                           ; 100h -> 16 * sizeof(xmm register)
    movaps xmmword ptr [rsp], xmm0
    movaps xmmword ptr [rsp + 10h], xmm1
    movaps xmmword ptr [rsp + 20h], xmm2
    movaps xmmword ptr [rsp + 30h], xmm3
    movaps xmmword ptr [rsp + 40h], xmm4
    movaps xmmword ptr [rsp + 50h], xmm5
    movaps xmmword ptr [rsp + 60h], xmm6
    movaps xmmword ptr [rsp + 70h], xmm7
    movaps xmmword ptr [rsp + 80h], xmm8
    movaps xmmword ptr [rsp + 90h], xmm9
    movaps xmmword ptr [rsp + 0a0h], xmm10
    movaps xmmword ptr [rsp + 0b0h], xmm11
    movaps xmmword ptr [rsp + 0c0h], xmm12
    movaps xmmword ptr [rsp + 0d0h], xmm13
    movaps xmmword ptr [rsp + 0e0h], xmm14
    movaps xmmword ptr [rsp + 0f0h], xmm15

    push 0      ; placeholder for rflags
    push 0      ; placeholder for rip
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push 0      ; placeholder for rsp
    push rbx
    push rdx
    push rcx
    push rax

    mov rdx, rsp
    mov rcx, qword ptr [rsp + 190h]
    sub rsp, 20h    ; allocate shadow space
    call ?dispatch@mshv_vmexit_handler@vmx@siren@@CA_NPEAVmshv_virtual_cpu@23@PEAUguest_state_t@23@@Z
    add rsp, 20h

    test al, al
    jz stop_virtualization
        pop rax
        pop rcx
        pop rdx
        pop rbx
        add rsp, 8h     ; skip rsp
        pop rbp
        pop rsi
        pop rdi
        pop r8
        pop r9
        pop r10
        pop r11
        pop r12
        pop r13
        pop r14
        pop r15
        add rsp, 10h     ; skip rip, rflags

        movaps xmm0, xmmword ptr [rsp]
        movaps xmm1, xmmword ptr [rsp + 10h]
        movaps xmm2, xmmword ptr [rsp + 20h]
        movaps xmm3, xmmword ptr [rsp + 30h]
        movaps xmm4, xmmword ptr [rsp + 40h]
        movaps xmm5, xmmword ptr [rsp + 50h]
        movaps xmm6, xmmword ptr [rsp + 60h]
        movaps xmm7, xmmword ptr [rsp + 70h]
        movaps xmm8, xmmword ptr [rsp + 80h]
        movaps xmm9, xmmword ptr [rsp + 90h]
        movaps xmm10, xmmword ptr [rsp + 0a0h]
        movaps xmm11, xmmword ptr [rsp + 0b0h]
        movaps xmm12, xmmword ptr [rsp + 0c0h]
        movaps xmm13, xmmword ptr [rsp + 0d0h]
        movaps xmm14, xmmword ptr [rsp + 0e0h]
        movaps xmm15, xmmword ptr [rsp + 0f0h]
        add rsp, 100h
        vmresume
        int 3
stop_virtualization:
        mov rax, qword ptr [rsp]
        mov rcx, qword ptr [rsp + 8h]
        mov rdx, qword ptr [rsp + 10h]
        mov rbx, qword ptr [rsp + 18h]
        ; skip rsp
        mov rbp, qword ptr [rsp + 28h]
        mov rsi, qword ptr [rsp + 30h]
        mov rdi, qword ptr [rsp + 38h]
        mov r8, qword ptr [rsp + 40h]
        mov r9, qword ptr [rsp + 48h]
        mov r10, qword ptr [rsp + 50h]
        mov r11, qword ptr [rsp + 58h]
        mov r12, qword ptr [rsp + 60h]
        mov r13, qword ptr [rsp + 68h]
        mov r14, qword ptr [rsp + 70h]
        ; skip r15, rip, rflags
        movaps xmm0, xmmword ptr [rsp + 90h]
        movaps xmm1, xmmword ptr [rsp + 0a0h]
        movaps xmm2, xmmword ptr [rsp + 0b0h]
        movaps xmm3, xmmword ptr [rsp + 0c0h]
        movaps xmm4, xmmword ptr [rsp + 0d0h]
        movaps xmm5, xmmword ptr [rsp + 0e0h]
        movaps xmm6, xmmword ptr [rsp + 0f0h]
        movaps xmm7, xmmword ptr [rsp + 100h]
        movaps xmm8, xmmword ptr [rsp + 110h]
        movaps xmm9, xmmword ptr [rsp + 120h]
        movaps xmm10, xmmword ptr [rsp + 130h]
        movaps xmm11, xmmword ptr [rsp + 140h]
        movaps xmm12, xmmword ptr [rsp + 150h]
        movaps xmm13, xmmword ptr [rsp + 160h]
        movaps xmm14, xmmword ptr [rsp + 170h]
        movaps xmm15, xmmword ptr [rsp + 180h]

        mov r15, rsp
        mov rsp, qword ptr [r15 + 20h]
        push qword ptr [r15 + 80h]
        push qword ptr [r15 + 88h]
        push qword ptr [r15 + 78h]
        pop r15
        popfq
        ret
?entry_point@mshv_vmexit_handler@vmx@siren@@SAXXZ ENDP

?microsoft_hypercall@mshv_vmexit_handler@vmx@siren@@CA?AUresult_value_t@hypercalls@microsoft_hv@3@PEBXUinput_value_t@563@_K2@Z PROC
    mov rax, rcx
    mov rcx, rdx
    mov rdx, r8
    mov r8, r9
    call rax
    ret
?microsoft_hypercall@mshv_vmexit_handler@vmx@siren@@CA?AUresult_value_t@hypercalls@microsoft_hv@3@PEBXUinput_value_t@563@_K2@Z ENDP

?microsoft_fast_hypercall_ex@mshv_vmexit_handler@vmx@siren@@CA?AUresult_value_t@hypercalls@microsoft_hv@3@PEBXUinput_value_t@563@PEAX@Z PROC
    push rbx
    mov rax, rcx
    mov rcx, rdx
    mov rbx, r8
    
    mov rdx, qword ptr [rbx]
    mov r8, qword ptr [rbx + 8h]
    movaps xmm0, xmmword ptr [rbx + 10h]
    movaps xmm1, xmmword ptr [rbx + 20h]
    movaps xmm2, xmmword ptr [rbx + 30h]
    movaps xmm3, xmmword ptr [rbx + 40h]
    movaps xmm4, xmmword ptr [rbx + 50h]
    movaps xmm5, xmmword ptr [rbx + 60h]

    call rax

    movaps xmmword ptr [rbx + 60h], xmm5
    movaps xmmword ptr [rbx + 50h], xmm4
    movaps xmmword ptr [rbx + 40h], xmm3
    movaps xmmword ptr [rbx + 30h], xmm2
    movaps xmmword ptr [rbx + 20h], xmm1
    movaps xmmword ptr [rbx + 10h], xmm0
    mov qword ptr [rbx + 8h], r8
    mov qword ptr [rbx], rdx

    pop rbx
    ret
?microsoft_fast_hypercall_ex@mshv_vmexit_handler@vmx@siren@@CA?AUresult_value_t@hypercalls@microsoft_hv@3@PEBXUinput_value_t@563@PEAX@Z ENDP

END
