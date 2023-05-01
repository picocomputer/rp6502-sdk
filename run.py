import sys
sys.path.insert(1, 'rp6502')
import rp6502

if len(sys.argv) != 2:
    raise RuntimeError("missing filename")
filename = sys.argv[1]
addr = 0x200
tty = '/dev/ttyACM0'

mon=rp6502.Monitor(tty)
mon.send_break()
mon.send_file_to_memory(filename, addr)
mon.send_reset_vector()
mon.reset()
