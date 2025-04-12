FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential \
    manpages-dev \
    iproute2 \
    procps \
    htop \
    vim \
    nano \
    gcc \
    make \
    gdb \
    net-tools \
    iputils-ping \
    && apt clean

WORKDIR /workspace

CMD ["/bin/bash"]
