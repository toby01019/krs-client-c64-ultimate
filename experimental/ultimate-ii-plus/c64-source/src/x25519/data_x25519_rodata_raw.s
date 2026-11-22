.setcpu "6502"

; =============================================================================
; data_x25519_rodata_raw.s — the sibling's two initialized field constants,
; restated from libs/x25519/src/data.s. Type `ro`, never `bss`: ld65 drops
; init bytes from bss segments, which once left fe_p = 0 at runtime.
; The lookup tables upstream also keeps here are src/crypto/x25519_tables.s.
; =============================================================================

.export x25_basepoint, fe_p

.segment "X25519_RODATA"

        .align 32
x25_basepoint:
        .byte 9
        .res 31, 0
fe_p:
        .byte $ed
        .res 30, $ff
        .byte $7f

.assert (x25_basepoint & $1F) = 0, lderror, "x25_basepoint must be 32-byte aligned"
.assert (fe_p & $1F) = 0, lderror, "fe_p must be 32-byte aligned"