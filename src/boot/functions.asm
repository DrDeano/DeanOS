; Print a null terminated string to the screen
; ds:si is the location of the string
; Input:
;   si - pointer to the sring to be printed in register si
print_string_with_new_line:
    pusha				; Push all registers onto the stack
    mov     ah, 0x0e	; Specify the teletype output funtion
	xor		bx, bx
.loop:
    lodsb				; Load byte at address si into al and increment si
    cmp	    al, 0		; If it the end of the null-terminated string
    je      .done		; Then exit
    int     0x10		; Else print the character in al as an interupt into the BIOS
    jmp     short .loop ; Repeat for next character
.done:
	; Print the line feed and carrage return
	mov		al, 0x0a	; Teletype print sub function(0x0e), Line feed (0x0a)
    int     0x10
    mov     al, 0x0d	; Carriage return
    int     0x10
    popa				; Pop the register of the stack
    ret					; And return to caller

; Reboot the computer if there was an error
reboot:
	m_write_line reboot_msg
    
    xor 	ah, ah					; Sub funtion for reading a character
    int 	0x16     				; Wait for key press
    int 	0x19     				; Warm reboot
    
    cli         	 				; If failed to reboot, halt
    hlt								; Halt
	
; Read a sector from the disk
; es:bx is the location of the buffer that the data is read into
; As reads oftern fail, it will try 4 times to read from the disk. The counter is stored in cx.
; With the data buffer at es:bx
; Input:
;	ax    - The logical block address (LBA)
;	es:bx - The buffer location which the sector will be read into
read_sector:
	xor		cx, cx					; Set the counter to 0
.read:
	push	ax						; Save the logical block address
	push	cx						; Save the counter
	
	; Convert the logical block address into the head-cylinder/track-sector values
	
; The convertions are:
; (1) Sector   = (LBA mod SectorsPerTrack) + 1
; (2) Cylinder = (LBA / SectorsPerTrack) / NumHeads
; (3) Head     = (LBA / SectorsPerTrack) mod NumHeads
;
; Input:
;	ax - the logical block address
; Output: These are used for the 0x13 BIOS interupt to read from the disk along with es:bx and ax
;	ch - Lower 8 bits of cylinder
;	cl - Upper 2 bits of cylinder and 6 bits for the sector
;	dh - The head number
;	dl - The dirve number/ID
.lba_to_hcs:
	push	bx							; Save the buffer location
	
	;mov		bx, word [Sectors_per_track]	; Get the sectors per track
	xor		dx, dx						; Set dx to 0x0 (part of operand for DIV instruction and needs to be 0x0)
	div		word [Sectors_per_track]	; Divide (dx:ax / Sectors_per_track)
										; Quotient (ax)  - LBA / SectorsPerTrack
										; Remainder (dx) - LBA mod SectorsPerTrack
									
	inc		dx							; (1) Sector = (LBA mod SectorsPerTrack) + 1
	mov		cl, dl						; Store sector in cl as defined for the output and for the 0x13 BIOS interupt
	
	;mov		bx, word [Head_count]	; Get the number of heads
	xor		dx, dx
	div		word [Head_count]			; Quotient (ax)  - Cylinder = (LBA / SectorsPerTrack) / NumHeads
										; Remainder (dx) - Head     = (LBA / SectorsPerTrack) mod NumHeads
	
	mov		ch, al						; Store cylinder in ch as defined for the output and for the 0x13 BIOS interupt
	mov		dh, dl						; Store head in dh as defined for the output and for the 0x13 BIOS interupt
	
	mov		dl, byte [Logical_drive_number]	; Store dirve number in dl as defined for the output and for the 0x13 BIOS interupt
	
	pop		bx							; Restore the buffer location
	
	; Using the values above, read off the drive
	
	mov		ax, 0x0201				; Sub funtion 2 to read from the disk, Read 1 (0x01) sector
	int		0x13					; Call BIOS interupt 13h
	jc		short .read_fail		; If fails to read (carry bit set)

	pop		cx
	pop		ax						; Restore the logical block address
	ret								; If read successful, then return to caller
.read_fail:							; If failed to read, try again, if tryed 4 times, the reboot
	pop		cx						; Restore the counter
	inc		cx						; Increment the counter
	cmp		cx, 4					; Compare if counter is equal to 4
	je		boot_error				; If equal, then error reading 4 times so reboot
	xor		ah, ah					; Reset the disk to try again
	int		0x13
	
	pop		ax						; Restore the logical block address
	jmp		.read					; Try to read again


