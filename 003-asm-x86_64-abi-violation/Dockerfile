FROM debian:bookworm-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        nasm \
        binutils \
        gdb \
        make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
CMD ["bash"]
