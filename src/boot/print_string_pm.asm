[bits 32]
; print_string_pm function. prints a null-terminated string pointed to by ebx
print_string_pm:
  pusha
  mov edx, 0xb8000         ; Let edx point to the start of video memory
	mov ah, 0x00        ; store attributes in ah
print_string_pm_loop:
  mov al, [ebx]                 ; store character in al
  
  cmp al, 0x0
  je print_string_pm_done       ; jump to done when we've reached a null byte

  mov [edx], ax                 ; store char and attributes at the current vid mem cell
  add ah, 1
  add ebx, 1                    ; point to next character in string
  add edx, 2                    ; point to next cell in vid mem

  jmp print_string_pm_loop      ; loop until we reach a null byte

print_string_pm_done:
  popa
  ret
