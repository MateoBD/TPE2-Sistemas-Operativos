# Run this script only the fist time then it's unnecessary
if [ -f /etc/fedora-release ]; then
  VOLUME_OPTION=":Z"
else
  VOLUME_OPTION=""
fi
sudo docker pull agodio/itba-so-multi-platform:3.0
sudo docker run -d -v ${PWD}:/root$VOLUME_OPTION --security-opt seccomp:unconfined -it --name TPE2-SO agodio/itba-so-multi-platform:3.0
