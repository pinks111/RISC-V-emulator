import struct

words = [
    0x04000893,  # addi x17, x0, 64   (write)
    0x00100513,  # addi x10, x0, 1    (stdout)
    0x02000593,  # addi x11, x0, 32   (адрес строки)
    0x00300613,  # addi x12, x0, 3    (длина)
    0x00000073,  # ecall
    0x05D00893,  # addi x17, x0, 93   (exit)
    0x00000513,  # addi x10, x0, 0    (код возврата)
    0x00000073,  # ecall
]
data = b"".join(struct.pack("<I", w) for w in words) + b"Hi\n"

with open("examples/hello.bin", "wb") as f:
    f.write(data)