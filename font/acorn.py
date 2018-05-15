

# acorn-electron-8x8.bin from https://maze.io/piece/font/computer/acorn-computers/

import sys
from PIL import Image

if len(sys.argv) < 3:
    print('USAGE: {} [input filename] [output filename]'.format(sys.argv[0]))
    sys.exit(-1)

infile = sys.argv[1]
outfile = sys.argv[2]

with open(infile, 'rb') as f:
    raw = f.read()

nletters = len(raw) // 8
print(nletters)
letters = []
for i in range(nletters):
    letters.append(raw[(i*8):(i*8+8)])
#print(letters)
assert all(len(l) == 8 for l in letters)

w = 16
h = nletters // w
size = (w*8, h*8)
imgraw = [None for _ in range(w*h*8*8)]
for row in range(h*8):
    for col in range(w*8):
        letter = letters[(row // 8)*w + (col // 8)]
        lrow = (row % 8)
        lcol = (col % 8)
        if letter[lrow] & (1 << (7-lcol)):
            imgraw[row*w*8+col] = 0xff
        else:
            imgraw[row*w*8+col] = 0

assert all(x is not None for x in imgraw)
imgraw = bytes(imgraw)

img = Image.frombytes('L', size, imgraw)
#img.show()

box = (0, 0, w*8, h*8/2)
top = img.crop(box=box)
box = (0, h*8/2, w*8, h*8)
bot = img.crop(box=box)

#top.show()
#bot.show()

img.save(infile + '.png')
top.save(infile + '-black.png')
bot.save(infile + '-white.png')

# Rotate sequence of 8 bytes, e.g.
# 1100 0011        0101 0101
# 0011 1100        1001 1001
# 1010 0101        0110 0110
# 0101 1010   = \  1010 1010
# 1100 0011   = /  1010 1010
# 0011 1100        0110 0110
# 1010 0101        1001 1001
# 0101 1010        0101 0101
def rotate(bs):
    out = bytearray(len(bs))
    for i in range(8):
        for j in range(8):
            #out[i] |= ((bs[7-j] >> (7-i)) & 1) << (7-j)
            out[i] <<= 1
            out[i] |= (bs[7-j] >> (7-i)) & 1
    return bytes(out)

bs = bytes([
    0b11000011,
    0b00111100,
    0b10100101,
    0b01011010,
    0b11000011,
    0b00111100,
    0b10100101,
    0b01011010,
    ])
expected = bytes([
    0b01010101,
    0b10011001,
    0b01100110,
    0b10101010,
    0b10101010,
    0b01100110,
    0b10011001,
    0b01010101,
    ])

observed = rotate(bs)
print([bin(o) for o in observed])
assert rotate(bs) == expected

#letters = letters[128+32:]
letters = letters[32:128]
rawout = (rotate(l) for l in letters)
rawout = ('{{ {} }}'.format(', '.join(map(hex, l))) for l in rawout)

# ` maps to £
# \t maps to a square black block
chars = []
chars.extend(' !"#$%&\'()*+,-./')
chars.extend('0123456789:;<=>?')
chars.extend('@ABCDEFGHIJKLMNO')
chars.extend('PQRSTUVWXYZ[\\]^_')
chars.extend('`abcdefghijklmno')
chars.extend('pqrstuvwxyz{|}~\t')

ascii_lut = [0 for _ in range(len(chars))]
for i, c in enumerate(chars):
    ascii_lut[ord(c)-32] = i


C_TEMPLATE = """#include <stdint.h>

const uint8_t font8x8[{}][8] = {{
  {}
}};

const uint8_t font8x8_lut[{}] = {{
  {}
}};
"""
with open(outfile, 'w') as f:
    f.write(C_TEMPLATE.format(len(letters), ',\n  '.join(rawout), len(letters),
                              ', '.join(map(str, ascii_lut))))

