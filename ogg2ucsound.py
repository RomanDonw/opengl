import os
import sys
import struct

import soundfile

UCSOUND_VERSION = 0

UCSOUND_TYPE_MONO_U8 = 0
UCSOUND_TYPE_MONO_S16 = 1
UCSOUND_TYPE_STEREO_U8 = 2
UCSOUND_TYPE_STEREO_S16 = 3

if len(sys.argv) >= 2 + 1:
    if os.path.isfile(sys.argv[1]):
        if os.path.isfile(sys.argv[2]):
            if input(f"Output file \"{sys.argv[2]}\" already exist. Overwrite it (type \"y\" and press Enter to confirm)? ").lower() != "y":
                print("Overwriting canceled.")
                exit(0)

        with soundfile.SoundFile(sys.argv[1]) as sfl:
            type = -1
            format_str = ""
            multiplier = -1

            if sfl.channels == 1 and sfl.subtype == "PCM_U8":
                type = UCSOUND_TYPE_MONO_U8
                format_str = "<B"
                multiplier = 127

            elif sfl.channels == 1 and sfl.subtype == "PCM_16":
                type = UCSOUND_TYPE_MONO_S16
                format_str = "<h"
                multiplier = 32767

            elif sfl.channels == 2 and sfl.subtype == "PCM_U8":
                type = UCSOUND_TYPE_STEREO_U8
                format_str = "<B"
                multiplier = 127

            elif sfl.channels == 2 and sfl.subtype == "PCM_16":
                type = UCSOUND_TYPE_STEREO_S16
                format_str = "<h"
                multiplier = 32767

            else:
                print(f"Sample type \"{sfl.subtype}\" or channels count {sfl.channels} doesn't supported for {sfl.format} audioformat.")
                exit(1)

            with open(sys.argv[2], "wb") as f:
                f.write(struct.pack("<7sHBH", b"UCSOUND", UCSOUND_VERSION, type, sfl.samplerate))

                for sample in sfl.read(dtype="float64"):
                    f.write(struct.pack(format_str, int((sample + 1) * multiplier)))

    else:
        print(f"Input file \"{sys.argv[1]}\" doesn't exist.")

else:
    print(f"Usage: {sys.argv[0]} <input .ogg file> <output UCSOUND file>")