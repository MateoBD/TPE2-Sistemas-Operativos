for arg in "$@"; do
  if [[ "$arg" == "buddy" ]]; then
    echo "Usando el buddy allocator"
    USE_BUDDY=1
    elif [[ "$arg" == "gdb" ]]; then
    echo "Usando el gdb"
    USE_GDB=1
  fi
done
if [ $USE_BUDDY ]; then
  echo "Usando el allocator por defecto"
  ./compile.sh buddy
else
  ./compile.sh
fi
if [ $USE_GDB ]; then
  echo "Usando el gdb"
  ./run.sh gdb
else
  ./run.sh
fi
