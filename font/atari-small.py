
import re
import sys


if len(sys.argv) < 3:
    print('USAGE: {} [input file] [output file]'.format(sys.argv[0]))
    sys.exit(-1)

infile = sys.argv[1]
outfile= sys.argv[2]

with open(infile, 'r') as f:
    lines = f.read().split('\n')

i = 0
while i < len(lines) and not lines[i].startswith('CHARS '):
    i += 1

nchars = int(lines[i].rstrip().split(' ')[-1])
i += 1
letters = [None for _ in range(nchars)]

# For each character, read the row-by-row pixel representation. This will
# need to be rotated to a columnar representation.
ordinal = None
in_bitmap = False
for line in lines[i:]:
    if line.startswith('ENDCHAR'):
        assert len(letters[ordinal]) == 8
        ordinal = None
        in_bitmap = False
    elif in_bitmap:
        letters[ordinal].append(int(line.rstrip(), 16))
    elif line.startswith('ENDFONT'):
        break
    elif line.startswith('STARTCHAR'):
        # Format is `STARTCHAR Cxxx` --- split + strip the C
        ascid = line.rstrip().split(' ')[-1]
        if len(ascid) > 1:
            # id is in octal
            ascid = ascid[1:]
            ordinal = int(ascid, 8)
        else:
            ordinal = ord(ascid)

        letters[ordinal] = []
    elif line.startswith('BITMAP'):
        in_bitmap = True

#zeros = [0 for _ in range(8)]
#for l in letters:
#    for b in l:
#        zeros[0] += (b >> 0) & 1
#        zeros[1] += (b >> 1) & 1
#        zeros[2] += (b >> 2) & 1
#        zeros[3] += (b >> 3) & 1
#        zeros[4] += (b >> 4) & 1
#        zeros[5] += (b >> 5) & 1
#        zeros[6] += (b >> 6) & 1
#        zeros[7] += (b >> 7) & 1



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
    out = bytearray(4)
    for i in range(4):
        for j in range(8):
            #out[i] |= ((bs[7-j] >> (7-i)) & 1) << (7-j)
            out[i] <<= 1
            out[i] |= (bs[7-j] >> (7-i)) & 1
    return bytes(out)

rawout = (rotate(l) for l in letters)
rawout = ('{{ {} }}'.format(', '.join(map(hex, l))) for l in rawout)


C_TEMPLATE = """#include <stdint.h>

const uint8_t font4x8[{}][4] = {{
  {}
}};
"""
with open(outfile, 'w') as f:
    f.write(C_TEMPLATE.format(len(letters), ',\n  '.join(rawout)))

