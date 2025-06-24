#!/bin/bash

# Set audio driver based on OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    AUDIO_DRIVER="pa"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    AUDIO_DRIVER="coreaudio"
else
    echo "Sistema operativo no soportado"
    exit 1
fi

if [[ "$1" == "gdb" ]]; then
    GDB="-s -S"
else
    GDB=""
fi


# Run QEMU with appropriate audio driver
qemu-system-x86_64 \
    -hda Image/x64BareBonesImage.qcow2 \
    -m 512 \
    -audiodev $AUDIO_DRIVER,id=speaker \
    -machine pcspk-audiodev=speaker \
    $GDB \
    -d int 2>&1 | grep "v="
