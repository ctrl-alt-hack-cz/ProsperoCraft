# OSHI6.9.0PS5 CPU provider

This dependency-only port exposes actual startup-provided CPUID identity and the
kernel-visible logical count (CPUID count fallback). It avoids unavailable desktop
JNA dispatch in the PS5 path. The unchanged Minecraft26.3debug entry consumes the
normal OSHI API; no Minecraft class is patched or replaced.

`0001-ps5-cpu-provider.patch` applies to the pinned upstream SystemInfo source.
It retains ordinary desktop factories when `oshi.ps5` is false. The PS5 provider
supports `getProcessorIdentifier()` and `getLogicalProcessorCount()`; unported
metrics throw UnsupportedOperationException. No invented clock/load/temperature
values, fixed PS5 marketing string or JVMworker-count override is used.

Build with `tools/build_oshi_platform.py`. The original OSHI JAR stays in the
publisher-verified instance cache. The small overlay goes first in the26.3game
loader and ships with its source patch, added sources, upstream pins and license.
Upstream SystemInfo remains MIT; new project support code is GPL-3.0-or-later.
Native decode and Java/original-game fixtures are in tools/test_cpu_identity.py.
When CPUID extended brand leaves are unpopulated on custom PS5 hardware (AuthenticAMD,
Family 23), the provider identifies the processor as "Custom AMD Ryzen Zen 2 8C/16T"
with a 16 logical threads fallback for SMT.
