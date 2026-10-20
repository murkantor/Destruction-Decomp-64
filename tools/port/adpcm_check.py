#!/usr/bin/env python3
"""DD64_AUDIO_DUMP checker. Re-decode every recorded ADPCM command with an independent decoder (the SDK's
vadpcm_dec formulation: coefficient matrix rows and an inner product) and
compare with what the port's interpreter produced."""
import struct, sys
d = open(sys.argv[1], 'rb').read()
pos = 0
recs = 0; bad = 0; firstbad = None; samples = 0; joins = 0; badjoins = 0
prev_out = None
def clamp(v):
    return -32768 if v < -32768 else 32767 if v > 32767 else v
while pos + 16 <= len(d):
    magic, flags, count, _ = struct.unpack_from('<IIII', d, pos)
    if magic != 0xADC0ADC0:
        print('bad magic at %d' % pos); break
    pos += 16
    state = list(struct.unpack_from('<16h', d, pos)); pos += 32
    book = list(struct.unpack_from('<128h', d, pos)); pos += 256
    nframes = count // 32
    inp = d[pos:pos + nframes * 9]; pos += nframes * 9
    out = list(struct.unpack_from('<%dh' % (count // 2), d, pos)); pos += count
    # continuity: a command that is neither init nor loop must start from the previous command's last frame
    if prev_out is not None and not (flags & 3):
        joins += 1
        if state != prev_out[-16:]:
            badjoins += 1
            if badjoins <= 3:
                print('wrong state at command %d (flags %d count %d, previous count %d): state %s, previous tail %s' % (
                    recs + 1, flags, count, prev_count, state[-4:], prev_out[-4:]))
    prev_count = count
    prev_out = out
    # independent decode: order 2, 8 rows per predictor; book[p*16 + k*8 + i] = coef k (0 = older) for row i
    ref = []
    hist = [state[14], state[15]]  # s[-2], s[-1]
    for f in range(nframes):
        hdr = inp[f * 9]
        scale = hdr >> 4; p = hdr & 0xF
        b0 = book[p * 16:p * 16 + 8]; b1 = book[p * 16 + 8:p * 16 + 16]
        nib = []
        for byte in inp[f * 9 + 1:f * 9 + 9]:
            nib += [byte >> 4, byte & 0xF]
        ix = [(v - 16 if v >= 8 else v) << min(scale, 12) for v in nib]  # the RSP caps the shift at 12
        for half in range(2):
            seg = ix[half * 8:half * 8 + 8]
            res = []
            for i in range(8):
                # row i of the matrix: [b0[i], b1[i], b1[i-1], ..., b1[0], 2048 at column i]
                acc = b0[i] * hist[0] + b1[i] * hist[1] + (seg[i] << 11)
                for j in range(i):
                    acc += b1[i - 1 - j] * seg[j]
                res.append(clamp(acc >> 11))
            hist = [res[6], res[7]]
            ref += res
    recs += 1; samples += len(out)
    if ref != out:
        bad += 1
        if firstbad is None:
            k = next(i for i in range(len(out)) if ref[i] != out[i])
            firstbad = (recs, k, ref[max(0, k - 2):k + 4], out[max(0, k - 2):k + 4], 'flags %d count %d hdr %02X' % (flags, count, inp[(k // 16) * 9] if inp else 0))
print('%d commands, %d samples: %d differ from the reference decode; %d joins between commands, %d with a wrong state' % (recs, samples, bad, joins, badjoins))
if firstbad:
    print('first difference: command %d sample %d: ref %s got %s (%s)' % firstbad)
