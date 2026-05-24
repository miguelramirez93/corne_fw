from PIL import Image, ImageOps
import sys

src = sys.argv[1]
target_h = int(sys.argv[2]) if len(sys.argv) > 2 else 32
threshold = int(sys.argv[3]) if len(sys.argv) > 3 else 80

img = Image.open(src).convert('RGBA')
white = Image.new('RGBA', img.size, (255, 255, 255, 255))
img = Image.alpha_composite(white, img).convert('L')
img = ImageOps.autocontrast(img, cutoff=5)
img = img.point(lambda p: 0 if p < threshold else 255).convert('1')

w, h = img.size
new_w = 32
new_h = max(1, round(h * new_w / w))
if new_h > target_h:
    new_h = target_h
    new_w = max(1, round(w * new_h / h))
icon = img.resize((new_w, new_h), Image.LANCZOS)
icon = icon.point(lambda p: 0 if p < 128 else 255).convert('1')

# Pack as row-major 32-wide bitmap, MSB-first per byte
canvas = Image.new('1', (32, target_h), 1)
x_off = (32 - new_w) // 2
canvas.paste(icon, (x_off, 0))

px = canvas.load()
print(f"// {new_w}x{new_h} bulldog, row-major MSB-first, 32 wide x {target_h} tall = {32 * target_h // 8} bytes")
print(f"static const uint8_t PROGMEM bulldog_pixels[{32 * target_h // 8}] = {{")
for row in range(target_h):
    line = "    "
    for col in range(0, 32, 8):
        byte = 0
        for bit in range(8):
            x = col + bit
            if px[x, row] == 0:
                byte |= (1 << (7 - bit))
        line += f"0x{byte:02X}, "
    print(line)
print("};")
print()
print(f"#define BULLDOG_W 32")
print(f"#define BULLDOG_H {target_h}")
