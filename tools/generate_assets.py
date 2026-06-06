#!/usr/bin/env python3
"""Generate launcher icons and per-level ambient music loops."""
import math
import os
import struct
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOUNDS = os.path.join(ROOT, "app", "src", "main", "assets", "sounds")
RES = os.path.join(ROOT, "app", "src", "main", "res")
ICON_BASE = os.path.join(
    os.path.dirname(ROOT),
    ".cursor", "projects", "c-Users-User-Downloads-RelicSeeker-main",
    "assets", "ic_launcher_base.png"
)
# Fallback if cursor path differs
if not os.path.exists(ICON_BASE):
    ICON_BASE = os.path.join(ROOT, "tools", "ic_launcher_base.png")

RATE = 44100


def write_wav(path, samples, rate=RATE):
    with wave.open(path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        frames = [max(-32768, min(32767, int(s))) for s in samples]
        w.writeframes(struct.pack("<" + "h" * len(frames), *frames))


def gen_loop(duration, bpm, root_freq, mood):
    """Procedural ambient loop — unique per level mood."""
    n = int(RATE * duration)
    beat = RATE * 60.0 / bpm
    out = []
    for i in range(n):
        t = i / RATE
        # Seamless loop: all oscillators use integer cycle counts
        cycles = max(1, int(duration * root_freq / 4))
        phase = 2 * math.pi * cycles * t / duration

        bass = math.sin(phase * (root_freq / 55.0)) * 0.35
        pad = math.sin(phase * (root_freq / 110.0 * 1.5)) * 0.2
        pad += math.sin(phase * (root_freq / 110.0 * 2.0) + 1.2) * 0.12

        arp_note = int(t * 4) % 5
        arp_freqs = [root_freq, root_freq * 1.25, root_freq * 1.5, root_freq * 1.75, root_freq * 2]
        arp = math.sin(2 * math.pi * arp_freqs[arp_note] * t) * 0.08

        pulse = (math.sin(2 * math.pi * t / (beat / RATE)) * 0.5 + 0.5)
        drum = math.sin(2 * math.pi * 80 * t) * pulse * mood.get("drum", 0.15)

        shimmer = math.sin(2 * math.pi * 1200 * t + math.sin(2 * math.pi * 3 * t)) * mood.get("shimmer", 0.03)

        env = 0.7 + 0.3 * math.sin(2 * math.pi * t / duration)
        s = (bass + pad + arp + drum + shimmer) * env * mood.get("gain", 0.55)
        # Soft clip
        s = math.tanh(s * 1.4) * 22000
        out.append(s)
    return out


LEVEL_TRACKS = [
    ("music_level1.wav", 14.0, 72,  146.8, {"drum": 0.10, "shimmer": 0.02, "gain": 0.50}),  # Temple jungle D
    ("music_level2.wav", 14.0, 80,  110.0, {"drum": 0.22, "shimmer": 0.01, "gain": 0.52}),  # Catacombes A
    ("music_level3.wav", 14.0, 68,  98.0,  {"drum": 0.08, "shimmer": 0.06, "gain": 0.48}),  # Crypte G
    ("music_level4.wav", 12.0, 96,  123.5, {"drum": 0.28, "shimmer": 0.02, "gain": 0.55}), # Pieges B
    ("music_level5.wav", 14.0, 76,  103.8, {"drum": 0.18, "shimmer": 0.04, "gain": 0.52}),  # Serpent G#m
    ("music_level6.wav", 14.0, 64,  87.3,  {"drum": 0.25, "shimmer": 0.01, "gain": 0.54}), # Anubis F
    ("music_level7.wav", 14.0, 88,  164.8, {"drum": 0.12, "shimmer": 0.08, "gain": 0.50}), # Cristal E
    ("music_level8.wav", 16.0, 108, 73.4, {"drum": 0.35, "shimmer": 0.05, "gain": 0.58}),  # Boss D
    ("music_menu.wav",   18.0, 70,  130.8, {"drum": 0.06, "shimmer": 0.03, "gain": 0.45}),  # Menu C
]


def generate_music():
    os.makedirs(SOUNDS, exist_ok=True)
    for fname, dur, bpm, root, mood in LEVEL_TRACKS:
        path = os.path.join(SOUNDS, fname)
        write_wav(path, gen_loop(dur, bpm, root, mood))
        print(f"music: {fname} ({os.path.getsize(path)} bytes)")


def generate_icons():
    from PIL import Image, ImageDraw, ImageFilter

    sizes = {
        "mipmap-mdpi": 48,
        "mipmap-hdpi": 72,
        "mipmap-xhdpi": 96,
        "mipmap-xxhdpi": 144,
        "mipmap-xxxhdpi": 192,
    }

    if os.path.exists(ICON_BASE):
        base = Image.open(ICON_BASE).convert("RGBA")
        # Centre-crop carre pour remplir tout le launcher
        w, h = base.size
        side = min(w, h)
        left = (w - side) // 2
        top = (h - side) // 2
        base = base.crop((left, top, left + side, top + side))
        base = base.resize((512, 512), Image.Resampling.LANCZOS)
    else:
        base = Image.new("RGBA", (512, 512), (12, 18, 32, 255))
        draw = ImageDraw.Draw(base)
        draw.ellipse([56, 56, 456, 456], fill=(20, 30, 50, 255))
        draw.polygon([(256, 100), (356, 280), (256, 400), (156, 280)], fill=(220, 180, 40, 255))
        draw.ellipse([226, 230, 286, 290], fill=(40, 180, 120, 255))
        print("warning: using fallback icon (base not found)")

    # Vignette legere sur les bords pour lisibilite
    overlay = Image.new("RGBA", base.size, (0, 0, 0, 0))
    od = ImageDraw.Draw(overlay)
    od.ellipse([16, 16, 496, 496], fill=(0, 0, 0, 0), outline=(255, 200, 60, 40), width=6)
    base = Image.alpha_composite(base, overlay)

    for folder, size in sizes.items():
        out_dir = os.path.join(RES, folder)
        os.makedirs(out_dir, exist_ok=True)
        icon = base.resize((size, size), Image.Resampling.LANCZOS)
        # Slight sharpen for small sizes
        if size <= 96:
            icon = icon.filter(ImageFilter.SHARPEN)
        icon.save(os.path.join(out_dir, "ic_launcher.png"), "PNG")
        print(f"icon: {folder}/ic_launcher.png ({size}x{size})")

    colors_dir = os.path.join(RES, "values")
    os.makedirs(colors_dir, exist_ok=True)
    colors_path = os.path.join(colors_dir, "colors.xml")
    if not os.path.exists(colors_path):
        with open(colors_path, "w", encoding="utf-8") as f:
            f.write('''<?xml version="1.0" encoding="utf-8"?>
<resources>
    <color name="ic_launcher_background">#0C1220</color>
</resources>
''')


if __name__ == "__main__":
    generate_icons()
    generate_music()
    print("Done.")
