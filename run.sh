#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # Para Linux
  qemu-system-x86_64 \
    -hda Image/x64BareBonesImage.qcow2 \
    -m 512 \
    -audiodev pa,id=speaker \
    -machine pcspk-audiodev=speaker
elif [[ "$OSTYPE" == "darwin"* ]]; then
  # Para Mac
  qemu-system-x86_64 \
    -hda Image/x64BareBonesImage.qcow2 \
    -m 512 \
    -audiodev coreaudio,id=speaker \
    -machine pcspk-audiodev=speaker
else
  echo "Sistema operativo no soportado"
  exit 1
fi
