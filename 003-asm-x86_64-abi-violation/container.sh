#!/bin/sh

docker build --platform linux/amd64 -t x86-abi-lab .
docker run --rm -it --platform linux/amd64 \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  -v "$PWD:/work" x86-abi-lab