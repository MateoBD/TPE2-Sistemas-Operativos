# Run this script only the fist time then it's unnecessary
sudo docker pull agodio/itba-so-multi-platform:3.0
sudo docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name TPE2-SO agodio/itba-so-multi-platform:3.0