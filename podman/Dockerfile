FROM debian:13-slim
RUN apt update; apt install -y git cmake build-essential python3 curl; apt clean all
RUN curl -fsSL https://raw.githubusercontent.com/picocomputer/.github/main/install/cc65.sh | sh; echo 'export PATH="$HOME/.rp6502/cc65/bin:$PATH"' >> ~/.bashrc
WORKDIR /rp6502

