from PIL import Image

img = Image.open('/tmp/fib_spiral.png').convert('1')
W, H = img.size
assert W == 32 and H == 32
px = img.load()

bytes_out = []
for row in range(H):
    for col_byte in range(4):
        byte = 0
        for bit in range(8):
            x = col_byte * 8 + bit
            if px[x, row] == 0:
                byte |= (1 << (7 - bit))
        bytes_out.append(byte)

print(f"// 32x32 Fibonacci (golden) spiral, row-major MSB-first")
print(f"static const uint8_t PROGMEM fib_spiral_pixels[{len(bytes_out)}] = {{")
for row in range(H):
    line = "    " + ", ".join(f"0x{bytes_out[row*4+c]:02X}" for c in range(4)) + ","
    print(line)
print("};")
