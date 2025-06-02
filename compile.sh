docker start TPE2-SO
docker exec -it TPE2-SO make clean -C /root/Toolchain
docker exec -it TPE2-SO make clean -C /root/

# Check if the argument is "buddy" to use buddy memory manager
if [ "$1" = "buddy" ]; then
    echo "Compiling with buddy memory manager..."
    docker exec -it TPE2-SO make -C /root/Toolchain
    docker exec -it TPE2-SO make -C /root/ buddy
else
    echo "Compiling with default memory manager..."
    docker exec -it TPE2-SO make -C /root/Toolchain
    docker exec -it TPE2-SO make -C /root/ all
fi

docker exec -it TPE2-SO chmod 777 /root/Image/x64BareBonesImage.qcow2 2>/dev/null || true
# docker stop TPE2-SO
