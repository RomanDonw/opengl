import sys
import os
import struct
from PIL import Image

UCTEX_VERSION = 0
UCTEX_TYPE_RGBA8 = 0
UCTEX_TYPE_RGB8 = 1
UCTEX_TYPE_RGB5_A1 = 2
UCTEX_TYPE_RGB5 = 3

if len(sys.argv) >= 2 + 1:
    if os.path.isfile(sys.argv[1]):
        if os.path.isfile(sys.argv[2]):
            if input(f"Output file \"{sys.argv[2]}\" already exist. Overwrite it (type \"y\" to confirm)? ").lower() != "y":
                print("Overwriting canceled.")
                exit(0)

        with Image.open(sys.argv[1]) as i:
            with open(sys.argv[2], "wb") as f:
                width, height = i.size

                f.write(struct.pack("5s", b"UCTEX"))
                f.write(struct.pack("<H", UCTEX_VERSION))

                match i.mode:
                    case "RGBA":
                        f.write(struct.pack("<B", UCTEX_TYPE_RGBA8))
                        f.write(struct.pack("<H", width - 1))
                        f.write(struct.pack("<H", height - 1))

                        for y in range(height):
                            for x in range(width):
                                pixel = i.getpixel((x, y))
                                f.write(struct.pack("<B", pixel[0]))
                                f.write(struct.pack("<B", pixel[1]))
                                f.write(struct.pack("<B", pixel[2]))
                                f.write(struct.pack("<B", pixel[3]))

                    case "RGB":
                        f.write(struct.pack("<B", UCTEX_TYPE_RGB8))
                        f.write(struct.pack("<H", width - 1))
                        f.write(struct.pack("<H", height - 1))

                        for y in range(height):
                            for x in range(width):
                                pixel = i.getpixel((x, y))
                                f.write(struct.pack("<B", pixel[0]))
                                f.write(struct.pack("<B", pixel[1]))
                                f.write(struct.pack("<B", pixel[2]))
    
    else:
        print(f"Input file \"{sys.argv[1]}\" doesn't exist.")

else:
    print(f"Usage: {sys.argv[0]} <input image> <output UCTEX file>")