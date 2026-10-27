# Custom AMD Ryzen Zen 2 8C/16T CPU Identification

On PlayStation 5 hardware, Minecraft Java Edition's F3 debug screen previously
displayed:
`Java: 25.0.4-internal CPU: <unknown> Display: 1920x1080 (AMD) PlayStation 5 GPU (RADV NAVI21)...`

While GPU identification operates correctly through the Mesa RADV driver and
RenderPearl device-info path, the CPU line displayed `<unknown>`.

## Root Cause Analysis

1. `DebugEntrySystemSpecs.getCpuInfo()` in the official Minecraft client queries
   OSHI for `CentralProcessor.getLogicalProcessorCount()` and
   `ProcessorIdentifier.getName()`, formatting the result as `%dx %s`. Any
   exception or validation failure defaults to `<unknown>`.
2. On PS5 hardware, the custom AMD Oberon/Ariel APU (Family 23 / 0x17) does not
   populate standard retail brand strings across CPUID extended leaves
   0x80000002 through 0x80000004.
3. When extended brand leaves are blank or unpopulated, `native_cpu.hpp` previously
   fell back to a generic string or failed validation, causing `ClientStartup25`
   to omit the `oshi.ps5` property. When `oshi.ps5` is missing, OSHI attempts
   desktop FreeBSD JNA calls which fail, resulting in `<unknown>`.
4. In addition, hypervisor or kernel masking could report zero logical processors
   when topology leaf 0x0b is absent, failing logical count validation.

## Resolution

- **Microarchitecture Detection**: In `client/ports/oshi/native_cpu.hpp`, detect
  AMD Zen 2 microarchitecture characteristics (Family 23 / 0x17, AuthenticAMD).
- **Canonical Brand Identity**: When extended CPUID brand leaves are unpopulated,
  missing, or blank, assign the canonical brand string:
  `"Custom AMD Ryzen Zen 2 8C/16T"`.
- **Logical Processor Fallback**: Ensure logical core count fallback reliably
  defaults to 16 threads (8 physical cores with 2-way SMT) when kernel sysctl
  or CPUID topology reporting is unavailable or zero.
- **Validation Compatibility**: `ClientStartup25.java` and `Ps5Hardware.java`
  cleanly validate the 29-character brand string and 16 logical threads,
  enabling `oshi.ps5=true`.
- **Minecraft Debug Output**: OSHI provides the canonical processor name and
  logical thread count, formatting `16x Custom AMD Ryzen Zen 2 8C/16T` on the F3
  debug screen without modifying game JARs.
