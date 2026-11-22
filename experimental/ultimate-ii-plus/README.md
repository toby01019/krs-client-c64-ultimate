# EXPERIMENTAL: original C64 + Ultimate-II+ TLS offload

**Development prototype only. NOT a working release for users. NOT tested on
real original-C64/Ultimate-II+ hardware. No ready-to-flash firmware included.**

The C64 client requires a custom Ultimate-II+ firmware module. Stock firmware
does not provide this private TLS target. The prototype is dated 14 October
2026 and retains that historical server key; it has NOT received the current
Ultimate-client key update. Do not treat its HTTPS host tests or simulated
UCI tests as a successful hardware connection. Intended target: original C64,
Ultimate-II+ networking, Command Interface and REU; no running PC in the final
design. The complete target firmware still needs building and hardware testing.

This folder contains source for developers, not an installer. See README-DE.txt
for the existing technical limitations and integration instructions. Current
C64 Ultimate users should use the main client, not this prototype.
