from PIL import Image, ImageOps
import sys

src = sys.argv[1]
target_h = int(sys.argv[2]) if len(sys.argv) > 2 else 32
threshold = int(sys.argv[3]) if len(sys.argv) > 3 else 100

img = Image.open(src).convert('RGBA')
white = Image.new('RGBA', img.size, (255, 255, 255, 255))
img = Image.alpha_composite(white, img).convert('L')
img = ImageOps.autocontrast(img, cutoff=5)
img = img.point(lambda p: 0 if p < threshold else 255).convert('1')

w, h = img.size
new_h = target_h
new_w = max(1, round(w * new_h / h))
if new_w > 32:
    new_w = 32
    new_h = max(1, round(h * new_w / w))
icon = img.resize((new_w, new_h), Image.LANCZOS)
icon = icon.point(lambda p: 0 if p < 128 else 255).convert('1')

portrait = Image.new('1', (32, 128), 1)
x_off = (32 - new_w) // 2
portrait.paste(icon, (x_off, 0))

native = portrait.transpose(Image.ROTATE_90)
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

print("// preview (32 wide x 128 tall portrait):")
print("//    " + "".join(f"{c%10}" for c in range(32)))
portrait_px = portrait.load()
for row in range(0, 40):
    line = "".join("#" if portrait_px[col, row] == 0 else "." for col in range(32))
    print(f"// {row:3} {line}")
print()
print("static const char PROGMEM bulldog_top[] = {")
for i in range(0, len(out), 16):
    chunk = out[i:i+16]
    line = ", ".join(f"0x{b:02X}" for b in chunk)
    print(f"    {line},")
print("};")
