.CODE

?turn_off_vm@siren_hypercalls@vmx@siren@@YAXXZ PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 1h
    vmcall

    pop rbx
    ret
?turn_off_vm@siren_hypercalls@vmx@siren@@YAXXZ ENDP

END