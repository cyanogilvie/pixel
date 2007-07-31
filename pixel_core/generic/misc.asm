	BITS 32
	
	GLOBAL asm_pelset
	GLOBAL asm_buf_draw_box
	
	EXTERN r_mask
	EXTERN g_mask
	EXTERN b_mask
	EXTERN r_shift
	EXTERN b_shift
	EXTERN g_shift
	EXTERN abs_r_sh
	EXTERN abs_g_sh
	EXTERN abs_b_sh

;	SECTION .text


; asm_pelset(_pel *dest, _pel colour, uint32 count)
asm_pelset:
			pushaf
			mov		edi,[esp+4]
			mov		eax,[esp+8]
			mov		ecx,[esp+12]
			cld
			rep		stosd
			popaf
			ret


xasm_buf_draw_box:
			; edi = ((y2 - y1 + 1) * width + x1) * 4 + dest_buf
			mov		eax,[esp+24]		; y2
			sub		eax,[esp+16]		; - y1
			inc		eax					; + 1
			mov		esi,[esp+8]
			imul	esi					; * image_width
			mov		ecx,[esp+12]
			add		eax,ecx				; + x1
			shl		eax,2				; * 4
			add		eax,[esp+4]			; + dest_buf
			mov		edi,eax

			mov		edx,[esp+20]		; x2
			sub		edx,ecx				; x1
			sub		esi,edx
			shl		esi,2				; esi = skip
			mov		eax,[esp+28]		; rgba col
			
			cld

; asm_buf_draw_box(_pel *dest_buf, int width, int x1, int y1, int x2, int y2, _pel rgba)
asm_buf_draw_box:
			ret
			mov		eax,[esp+16]		; y1
			mov		ebx,[esp+24]		; y2
			sub		ebx,eax
			inc		ebx
			mov		edx,[esp+8]			; width
			mov		edi,[esp+12]		; x1
			shl		edi,2
			mov		ecx,[esp+4]			; dest_buf -1 ?????
			dec		ecx
			add		edi,ecx
			imul	edx
			shl		eax,2
			add		edi,eax
			mov		edx,[esp+20]		; x2
			sub		edx,[esp+12]		; x1
			mov		esi,[esp+8]			; width
			sub		esi,edx
			shl		esi,2				; esi = skip
			mov		eax,[esp+28]		; rgba col
			cld

line:		mov		ecx,edx
			rep		stosd
			add		edi,esi
			dec		ebx
			jnz		line

			ret


