# 3_PirateOS

Dette er den aktive prosjektmappen for den nye versjonen av OS-et.

## Bygg

```bash
cmake -S src/3_PirateOS -B build/3_PirateOS
cmake --build build/3_PirateOS --target pirateos-create-image
```

## Kjoring

```bash
bash src/3_PirateOS/scripts/start_qemu.sh build/3_PirateOS/kernel.iso build/3_PirateOS/disk.iso
```

Som standard starter QEMU na med en gang. Hvis du vil stoppe ved boot for debugging:

```bash
QEMU_GDB=1 bash src/3_PirateOS/scripts/start_qemu.sh build/3_PirateOS/kernel.iso build/3_PirateOS/disk.iso
```

Hvis lyd ikke virker i containeroppsettet ditt, prov en annen backend:

```bash
QEMU_AUDIO_DRIVER=pa bash src/3_PirateOS/scripts/start_qemu.sh build/3_PirateOS/kernel.iso build/3_PirateOS/disk.iso
```
