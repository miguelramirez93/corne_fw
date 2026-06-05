"""Fibonacci-square spiral (Wikipedia-style: squares + quarter arcs).
Output: 32W x 42H 1-bit PNG, with the 26x42 spiral centered horizontally."""
from PIL import Image, ImageDraw

fib = [1, 1, 2, 3, 5, 8, 13]
scale = 2

DIRS = ['R', 'D', 'L', 'U']

squares = []
x, y = 0, 0
squares.append((x, y, fib[0], None))
bb = [x, y, x + fib[0], y + fib[0]]
di = 0
for i in range(1, len(fib)):
    s = fib[i]
    d = DIRS[di]
    if d == 'R':   nx, ny = bb[2], bb[1]
    elif d == 'D': nx, ny = bb[0], bb[3]
    elif d == 'L': nx, ny = bb[0] - s, bb[1]
    else:          nx, ny = bb[0], bb[1] - s
    squares.append((nx, ny, s, d))
    bb[0] = min(bb[0], nx); bb[1] = min(bb[1], ny)
    bb[2] = max(bb[2], nx + s); bb[3] = max(bb[3], ny + s)
    di = (di + 1) % 4

ox, oy = -bb[0], -bb[1]
W_logical = bb[2] - bb[0]
H_logical = bb[3] - bb[1]
squares = [(x + ox, y + oy, s, d) for x, y, s, d in squares]

# Output canvas is fixed 32 wide; horizontally center the spiral.
W_OUT = 32
H_OUT = H_logical * scale  # 42
x_off = (W_OUT - W_logical * scale) // 2  # (32 - 26)/2 = 3

img = Image.new('1', (W_OUT, H_OUT), 1)
draw = ImageDraw.Draw(img)

arc_spec = {
    'R': ('TR',  90, 180),
    'D': ('BR', 180, 270),
    'L': ('BL', 270, 360),
    'U': ('TL',   0,  90),
}

# Square outlines
for x, y, s, d in squares:
    sx, sy, ss = x * scale + x_off, y * scale, s * scale
    draw.rectangle([(sx, sy), (sx + ss - 1, sy + ss - 1)], outline=0, width=1)

# Arcs
for x, y, s, d in squares:
    if s == 1 or d is None:
        continue
    sx, sy, ss = x * scale + x_off, y * scale, s * scale
    corner, a0, a1 = arc_spec[d]
    if corner == 'TL':   cx, cy = sx, sy
    elif corner == 'TR': cx, cy = sx + ss, sy
    elif corner == 'BR': cx, cy = sx + ss, sy + ss
    else:                cx, cy = sx, sy + ss
    bbox = (cx - ss, cy - ss, cx + ss, cy + ss)
    draw.arc(bbox, a0, a1, fill=0, width=1)

print(f"output: {W_OUT}x{H_OUT}")
img.save('/tmp/fib_spiral.png')
img.resize((W_OUT * 10, H_OUT * 10), Image.NEAREST).save('/tmp/fib_spiral_preview.png')
