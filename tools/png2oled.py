from PIL import Image
import sys

src = sys.argv[1]
img = Image.open(src).convert('RGBA')
white = Image.new('RGBA', img.size, (255, 255, 255, 255))
img = Image.alpha_composite(white, img).convert('L')
img = img.point(lambda p: 0 if p < 200 else 255).convert('1')

w, h = img.size
new_w = 32
new_h = max(1, round(h * new_w / w))
if new_h > 128:
    new_h = 128
    new_w = max(1, round(w * new_h / h))
icon = img.resize((new_w, new_h), Image.LANCZOS)
icon = icon.point(lambda p: 0 if p < 128 else 255).convert('1')

portrait = Image.new('1', (32, 128), 1)
x_off = (32 - new_w) // 2
y_off = (128 - new_h) // 2
portrait.paste(icon, (x_off, y_off))

native = portrait.transpose(Image.ROTATE_270)

px = native.load()
out = []
for page in range(4):
    for x in range(128):
        byte = 0
        for bit in range(8):
            y = page * 8 + bit
            if px[x, y] == 0:
                byte |= (1 << bit)
        out.append(byte)

print("static const char PROGMEM corne_logo[] = {")
for i in range(0, len(out), 16):
    chunk = out[i:i+16]
    line = ", ".join(f"0x{b:02X}" for b in chunk)
    print(f"    {line},")
print("};")
