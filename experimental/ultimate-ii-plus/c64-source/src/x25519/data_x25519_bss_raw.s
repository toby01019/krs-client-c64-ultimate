.setcpu "6502"

; =============================================================================
; data_x25519_bss_raw.s — the sibling's 32-byte field buffers, restated from
; libs/x25519/src/data.s (which is not staged; see build_x25519.sh).
;
; 32-byte alignment is the whole contract (upstream docs/LIBRARY.md §6):
; every access is `abs,y` / `(zp),y` with Y in 0..31, so a 32-aligned buffer
; never crosses a page. Upstream additionally page-aligns the two groups of
; eight; that buys nothing the 32-byte rule does not, and here it would cost
; up to 255 B of fill per group. The asserts below are the contract.
; =============================================================================

.export fe25519_tmp1, fe25519_tmp2, fe25519_tmp3, fe25519_tmp4
.export x25_x2, x25_z2, x25_x3, x25_z3
.export x25_a, x25_b, x25_da, x25_cb, x25_e
.export x25_scalar, x25_u, x25_result, x25_x1

.segment "X25519_BSS"

        .align 32
fe25519_tmp1:   .res 32
fe25519_tmp2:   .res 32
fe25519_tmp3:   .res 32
fe25519_tmp4:   .res 32
x25_x2:         .res 32
x25_z2:         .res 32
x25_x3:         .res 32
x25_z3:         .res 32
x25_a:          .res 32
x25_b:          .res 32
x25_da:         .res 32
x25_cb:         .res 32
x25_e:          .res 32
x25_scalar:     .res 32
x25_u:          .res 32
x25_result:     .res 32
; RFC 7748 decoded u-coordinate (upstream #64, v0.7.0). x25519_scalarmult
; writes the bit-255-masked copy of x25_u here and the ladder's x_1 reads
; it. Dropping it fails the link on x25519.s's import, not silently.
x25_x1:         .res 32

;
; mul_dma_carry: the onchip profile never touches it, but fe25519_sqr's
; pre-doubled DMA bodies are assembled unconditionally and reference it,
; while their only entry (the trampoline patch) is inside `.if ::SQR_DMA_K`,
; which the onchip profile forces to 0. Upstream's mul_stage.s allocates
; 256 B for it anyway; here it is an alias so the dead reference links
; without costing a page. It is read-only in that dead code.
.import mul_dma_hi
.export mul_dma_carry
mul_dma_carry = mul_dma_hi

.assert (fe25519_tmp1 & $1F) = 0, lderror, "fe25519_tmp1 must be 32-byte aligned"
.assert (fe25519_tmp2 & $1F) = 0, lderror, "fe25519_tmp2 must be 32-byte aligned"
.assert (fe25519_tmp3 & $1F) = 0, lderror, "fe25519_tmp3 must be 32-byte aligned"
.assert (fe25519_tmp4 & $1F) = 0, lderror, "fe25519_tmp4 must be 32-byte aligned"
.assert (x25_x2 & $1F) = 0, lderror, "x25_x2 must be 32-byte aligned"
.assert (x25_z2 & $1F) = 0, lderror, "x25_z2 must be 32-byte aligned"
.assert (x25_x3 & $1F) = 0, lderror, "x25_x3 must be 32-byte aligned"
.assert (x25_z3 & $1F) = 0, lderror, "x25_z3 must be 32-byte aligned"
.assert (x25_a & $1F) = 0, lderror, "x25_a must be 32-byte aligned"
.assert (x25_b & $1F) = 0, lderror, "x25_b must be 32-byte aligned"
.assert (x25_da & $1F) = 0, lderror, "x25_da must be 32-byte aligned"
.assert (x25_cb & $1F) = 0, lderror, "x25_cb must be 32-byte aligned"
.assert (x25_e & $1F) = 0, lderror, "x25_e must be 32-byte aligned"
.assert (x25_scalar & $1F) = 0, lderror, "x25_scalar must be 32-byte aligned"
.assert (x25_u & $1F) = 0, lderror, "x25_u must be 32-byte aligned"
.assert (x25_result & $1F) = 0, lderror, "x25_result must be 32-byte aligned"
.assert (x25_x1 & $1F) = 0, lderror, "x25_x1 must be 32-byte aligned"