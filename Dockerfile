FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update \
    && apt install -qy \
        binutils \
        gcc \
        gdb \
        libc6-dev \
        make \
        sudo \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -G sudo -p '' user

USER user

WORKDIR /home/user