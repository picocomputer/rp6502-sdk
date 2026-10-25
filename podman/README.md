Podman Development
------------------
This is a file meant to make a developer container for creating rp6502 ROM images, such as those in this SDK template.
It follows the instructions found at https://picocomputer.github.io/sdk.html#linux

Run:
./create.sh

to create the Podman image.


Docker
------
Docker, all of this should also work with Docker, but has not been tested.


LLVM-MOS
--------
This was prototyped on an aarch64 computer, currently the LLVM-MOS installation script doesn't install on aarch64, it is x86_64 only while the cc65 one is not.

