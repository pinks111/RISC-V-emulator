import struct

words = [
    0x00000513,  # addi x10, x0, 0
    0x00100593,  # addi x11, x0, 1
    0x00B00613,  # addi x12, x0, 11
    0x00B50533,  # add  x10, x10, x11   (loop)
    0x00158593,  # addi x11, x11, 1
    0xFEC59CE3,  # bne  x11, x12, -8
    0x05D00893,  # addi x17, x0, 93
    0x00000073,  # ecall
]

data = b"".join(struct.pack("<I", w) for w in words)

with open("examples/sum.bin", "wb") as f:
    f.write(data)