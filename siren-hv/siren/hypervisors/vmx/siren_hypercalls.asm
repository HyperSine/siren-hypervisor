.CODE

?turn_off_vm@siren_hypercalls@vmx@hypervisors@siren@@YAXXZ PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 1h

    vmcall

    pop rbx
    ret
?turn_off_vm@siren_hypercalls@vmx@hypervisors@siren@@YAXXZ ENDP

?ept_commit_1gb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 2h
    
    vmcall

    pop rbx
    ret
?ept_commit_1gb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z ENDP

?ept_commit_2mb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 3h
    
    vmcall

    pop rbx
    ret
?ept_commit_2mb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z ENDP

?ept_commit_4kb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 4h
    
    vmcall

    pop rbx
    ret
?ept_commit_4kb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K0I@Z ENDP

?ept_uncommit_1gb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 5h
    
    vmcall

    pop rbx
    ret
?ept_uncommit_1gb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z ENDP

?ept_uncommit_2mb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 6h
    
    vmcall

    pop rbx
    ret
?ept_uncommit_2mb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z ENDP

?ept_uncommit_4kb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 7h
    
    vmcall

    pop rbx
    ret
?ept_uncommit_4kb_page@siren_hypercalls@vmx@hypervisors@siren@@YA?AUstatus_t@4@_K@Z ENDP

?ept_flush@siren_hypercalls@vmx@hypervisors@siren@@YAXXZ PROC
    push rbx

    mov eax, 'vhrs'
    mov ebx, 8h
    
    vmcall

    pop rbx
    ret
?ept_flush@siren_hypercalls@vmx@hypervisors@siren@@YAXXZ ENDP

END