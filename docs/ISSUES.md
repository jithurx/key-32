# Key32 Known Issues & Solutions

This document serves as a repository for tracking obscure hardware crashes, compiler quirks, and systemic bugs encountered during the development of the Key32 ESP32 Gaming Keychain, along with their solutions.

---

## 1. Hotspot QR Rendering: Core 1 Panic (LoadProhibited)

### Symptoms
When selecting the `HOTSPOT` menu option on the ESP32 to generate a custom WiFi Access Point and QR code, the system immediately crashed with a `Guru Meditation Error: Core 1 panic'ed (LoadProhibited)` and `EXCVADDR: 0x00000001`, forcing a hardware Watchdog reset.

### Root Cause
This was caused by a memory misalignment bug deeply tied to the Xtensa GCC compiler used by the Arduino IDE for the ESP32.
1. The **Richard Moore `qrcode` C library** (and our `hotspot.h` wrapper) utilized Variable Length Arrays (VLAs) to dynamically allocate memory on the stack (e.g., `uint8_t qrcodeData[qrcode_getBufferSize(version)]`).
2. When VLAs are evaluated concurrently with C++ objects that possess dynamic destructors (like the `String` class used to build the WiFi credential payload), the compiler miscalculates the stack frame base pointer.
3. When the function attempts to clean up the C++ `String` object upon loop exit, the ESP32 references a totally corrupted memory offset address (`0x00000001`) because the VLA offset the structural pointers natively, throwing a `LoadProhibited` hardware panic.

### Solution
1. **Library Vendoring**: We structurally vendored the `qrcode.c` and `qrcode.h` source files natively into the project and renamed them to `local_qrcode` to completely bypass the Arduino IDE accidentally linking the ESP32 Core's internal `qrcode.h`.
2. **Elimination of VLAs**: We refactored both `wifi/hotspot.h` and the deeply embedded `qrcode_initBytes` loops inside `local_qrcode.c` to completely replace Variable Length Arrays (which rely on dynamic `alloca()` handling) with **fixed-size, hardcoded byte arrays** (e.g., `uint8_t codewordBytes[512];`).

By explicitly telling the compiler the exact length the stack allocation arrays needed to be at compile time, we completely circumvented the dynamic Stack Pointer shifting, successfully stabilizing the ESP32 architecture and preventing the runtime crash.
