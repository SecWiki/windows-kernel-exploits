EXTERN OrginalUmsSchedulerAddress	:QWORD

.CODE
SetNonCanonicalAddress PROC FRAME
	mov r11, 8000000000000000h
	jmp [OrginalUmsSchedulerAddress]
	.ENDPROLOG
	ret
SetNonCanonicalAddress ENDP
END
