global outb;making the outb visible outside
global inb
;outb  - for sending a byte into I/O port
;stack: [esp+8] the data byte
;       [esp+4] the I/O port
;       [esp] ret
;inb - for receiving a byte from the given I/O port
;stack: [esp+4] the I/O port address
;       [esp] ret
outb:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret
inb:
    mov dx, [esp+4]
    in al, dx
    ret
