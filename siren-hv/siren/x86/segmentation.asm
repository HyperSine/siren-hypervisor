.CODE

??$read_segment_selector@$0A@@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, cs
	ret
??$read_segment_selector@$0A@@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$00@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, ss
	ret
??$read_segment_selector@$00@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$01@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, ds
	ret
??$read_segment_selector@$01@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$02@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, es
	ret
??$read_segment_selector@$02@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$03@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, fs
	ret
??$read_segment_selector@$03@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$04@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	mov ax, gs
	ret
??$read_segment_selector@$04@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$05@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	sldt ax
	ret
??$read_segment_selector@$05@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

??$read_segment_selector@$06@x86@siren@@YA?AUsegment_selector_t@01@XZ PROC
	str ax
	ret
??$read_segment_selector@$06@x86@siren@@YA?AUsegment_selector_t@01@XZ ENDP

?read_segment_unscrambled_limit@x86@siren@@YAIUsegment_selector_t@12@@Z PROC
	movzx ecx, cx
	xor eax, eax	; set eflags.zf = 1
	lsl eax, ecx
	jz ON_SUCCESS
	mov eax, NOT 0fffh
ON_SUCCESS:
	ret
?read_segment_unscrambled_limit@x86@siren@@YAIUsegment_selector_t@12@@Z ENDP

?read_segment_access_rights@x86@siren@@YA?AUsegment_access_rights_t@12@Usegment_selector_t@12@@Z PROC
	movzx ecx, cx
	lar eax, ecx
	jz ON_SUCCESS
	xor eax, eax
ON_SUCCESS:
	ret;
?read_segment_access_rights@x86@siren@@YA?AUsegment_access_rights_t@12@Usegment_selector_t@12@@Z ENDP

END
