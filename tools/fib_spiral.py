import math
from PIL import Image, ImageDraw

W, H = 32, 32
img = Image.new('1', (W, H), 1)
draw = ImageDraw.Draw(img)

b = math.log(1.618) / (math.pi / 2)
total_turns = 2.5
t_max = total_turns * 2 * math.pi
target_max_r = 14
a = target_max_r / math.exp(b * t_max)

cx, cy = W // 2, H // 2

prev = None
steps = 5000
for i in range(steps + 1):
    t = t_max * i / steps
    r = a * math.exp(b * t)
    x = cx + r * math.cos(t)
    y = cy + r * math.sin(t)
    if prev:
        draw.line([prev, (x, y)], fill=0, width=1)
    prev = (x, y)

img.save('/tmp/fib_spiral.png')
img.resize((W*10, H*10), Image.NEAREST).save('/tmp/fib_spiral_preview.png')
print(f"size {W}x{H}")
