#!/usr/bin/env bash

podman build -t rp6502_builder .

echo "Created rp6502_builder."
echo -ne "Run with \npodman run -it --rm -v $PWD/../:/rp6502:z rp6502_builder:latest\n\n"


