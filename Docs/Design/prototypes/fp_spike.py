"""THROWAWAY SPIKE: first-person colour raycaster frame of Main Street.

Renders a 160x80 pixel frame (160x40 terminal cells using the upper-half
block, 24-bit colour) plus a 4x PNG so the look can be checked.
"""
import math
import struct
import sys
import zlib

W, H = 160, 80
HORIZON = 34
FOV_PLANE = 0.66

# ---------------------------------------------------------------- town map
# Main Street runs east-west. Street rows y 4..8 are open. North buildings
# fill y 1..3 (facade on y=4 line), south buildings y 8..10 (facade y=8 line).
MAP_W, MAP_H = 40, 12
BUILDINGS = [
    # id, x0, x1 (inclusive), side, height, base colour, trim colour, style
    ("hotel", 2, 7, "N", 1.9, (150, 88, 62), (235, 220, 190), "planks"),
    ("saloon", 9, 14, "N", 1.6, (178, 125, 72), (90, 50, 30), "saloon"),
    ("gunsmith", 16, 19, "N", 1.3, (122, 98, 84), (210, 200, 170), "planks"),
    ("sheriff", 21, 25, "N", 1.4, (196, 176, 136), (110, 70, 40), "adobe"),
    ("stable", 27, 32, "N", 1.5, (142, 58, 44), (230, 220, 200), "barn"),
    ("church", 2, 6, "S", 2.3, (228, 226, 214), (120, 110, 100), "church"),
    ("store", 8, 14, "S", 1.5, (104, 132, 94), (235, 225, 195), "planks"),
    ("telegraph", 16, 20, "S", 1.3, (164, 150, 120), (70, 60, 50), "planks"),
    ("graves", 22, 29, "S", 0.38, (232, 230, 220), (60, 55, 50), "fence"),
]
grid = [[None] * MAP_W for _ in range(MAP_H)]
for b in BUILDINGS:
    _, x0, x1, side, *_ = b
    rows = range(1, 4) if side == "N" else range(8, 11)
    for y in rows:
        for x in range(x0, x1 + 1):
            grid[y][x] = b


def h32(a, b=0, c=0):
    n = (a * 374761393 + b * 668265263 + c * 2147483647) & 0xFFFFFFFF
    n = (n ^ (n >> 13)) * 1274126177 & 0xFFFFFFFF
    return (n ^ (n >> 16)) & 0xFFFFFFFF


def mul(c, k):
    return tuple(max(0, min(255, int(v * k))) for v in c)


def mix(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))


SKY_TOP = (38, 62, 128)
SKY_LOW = (244, 176, 112)
HAZE = (226, 170, 120)


# ------------------------------------------------------------ wall texture
def wall_texel(b, u, v, facade):
    bid, x0, x1, side, height, base, trim, style = b
    fu = u - math.floor(u)
    cell = int(math.floor(u))
    door_cell = (x0 + x1) // 2
    # roofline / false-front cap
    if v > height - 0.07:
        return mul(trim, 0.55)
    if style == "fence":
        picket = (fu * 5) % 1.0 < 0.55
        if v > height - 0.12 and not picket:
            return (40, 36, 30)
        return base if picket else (54, 46, 38)
    # sign board near the top
    if facade and style != "church" and height - 0.38 < v < height - 0.12:
        if 0.12 < (u - x0) / (x1 - x0 + 1) < 0.88:
            letter = h32(int(u * 7), int(v * 22)) % 3 == 0
            return (52, 34, 24) if letter else (226, 206, 158)
    # planks / adobe base
    if style == "adobe":
        c = mul(base, 0.92 + 0.08 * ((h32(int(u * 9), int(v * 9)) % 100) / 100))
    else:
        row = int(v * 9)
        c = mul(base, 0.86 + 0.14 * ((h32(row, cell) % 100) / 100))
        if (v * 9) % 1.0 < 0.13:
            c = mul(c, 0.72)
        if style in ("barn",) and (fu * 6) % 1.0 < 0.08:
            c = mul(c, 0.7)
    if not facade:
        return c
    # door
    if cell == door_cell and 0.22 < fu < 0.78 and v < 0.82:
        if style == "saloon" and not (0.22 < v < 0.62):
            return (34, 22, 16)
        if style == "barn":
            x_brace = abs((fu - 0.22) / 0.56 - v / 0.82) < 0.06 or \
                abs((fu - 0.22) / 0.56 - (1 - v / 0.82)) < 0.06
            return (230, 220, 200) if x_brace else (120, 46, 36)
        if fu < 0.27 or fu > 0.73 or v > 0.77:
            return trim
        return (70, 42, 28)
    # windows, one per cell, one or two storeys
    for lo, hi in ((0.38, 0.8), (1.05, 1.45)):
        if hi > height - 0.4:
            continue
        if style == "church" and lo > 1:
            lo, hi = 1.2, 1.9
        if 0.26 < fu < 0.74 and lo < v < hi:
            if fu < 0.31 or fu > 0.69 or v < lo + 0.05 or v > hi - 0.05:
                return trim
            glint = (fu - 0.31) + (v - lo) * 0.6
            return (150, 170, 190) if 0.18 < glint < 0.24 else (34, 44, 66)
    # porch shadow under the boardwalk roof
    if v < 0.1:
        return mul(c, 0.6)
    return c


# -------------------------------------------------------------- the render
def render(cam, cam_dir, cam_h, horse_at, mounted):
    px, py = cam
    dx, dy = cam_dir
    plx, ply = -dy * FOV_PLANE, dx * FOV_PLANE
    img = [[(0, 0, 0)] * W for _ in range(H)]
    zbuf = [1e9] * W

    # sky, sun, mountains
    for y in range(HORIZON):
        t = y / HORIZON
        row = mix(SKY_TOP, SKY_LOW, t ** 1.6)
        for x in range(W):
            img[y][x] = row
    sun_x, sun_y = W // 2 + 6, HORIZON - 9
    for y in range(HORIZON):
        for x in range(W):
            d = math.hypot(x - sun_x, (y - sun_y))
            if d < 6:
                img[y][x] = (255, 236, 186)
            elif d < 10:
                img[y][x] = mix(img[y][x], (255, 220, 160), (10 - d) / 8)
    for x in range(W):
        ridge = HORIZON - (5 + 3.5 * math.sin(x * 0.06 + 0.4) + 2.2 * math.sin(x * 0.17 + 2)
                           + 1.2 * math.sin(x * 0.41))
        for y in range(int(ridge), HORIZON):
            img[y][x] = mix((124, 86, 116), HAZE, 0.35 + 0.25 * (y - ridge) / 8)

    # ground (floor casting)
    for y in range(HORIZON, H):
        p = y - HORIZON + 0.5
        dist = cam_h * H / p
        for x in range(W):
            cx = 2 * x / W - 1
            wx = px + dist * (dx + plx * cx)
            wy = py + dist * (dy + ply * cx)
            n = (h32(int(wx * 6), int(wy * 6)) % 100) / 100
            in_town = wx < 33.5
            if 4.6 <= wy < 7.4 or not in_town:
                c = (178, 132, 86) if in_town else (214, 178, 126)
                if in_town and (abs(wy - 5.4) < 0.14 or abs(wy - 6.6) < 0.14):
                    c = mul(c, 0.82)
                c = mul(c, 0.95 + 0.06 * n)
            elif 4.0 <= wy < 4.6 or 7.4 <= wy < 8.0:
                c = (128, 88, 54)
                if (wx * 3) % 1.0 < 0.12:
                    c = mul(c, 0.65)
            else:
                c = mul((206, 170, 120), 0.95 + 0.05 * n)
            fog = min(1.0, dist / 34) ** 1.1 * 0.75
            img[y][x] = mix(c, HAZE, fog)

    # walls (DDA)
    for x in range(W):
        cx = 2 * x / W - 1
        rdx, rdy = dx + plx * cx, dy + ply * cx
        mx, my = int(px), int(py)
        ddx = abs(1 / rdx) if rdx else 1e30
        ddy = abs(1 / rdy) if rdy else 1e30
        sx, sdx = (-1, (px - mx) * ddx) if rdx < 0 else (1, (mx + 1 - px) * ddx)
        sy, sdy = (-1, (py - my) * ddy) if rdy < 0 else (1, (my + 1 - py) * ddy)
        hit, side = None, 0
        for _ in range(80):
            if sdx < sdy:
                sdx += ddx
                mx += sx
                side = 0
            else:
                sdy += ddy
                my += sy
                side = 1
            if not (0 <= mx < MAP_W and 0 <= my < MAP_H):
                break
            if grid[my][mx]:
                hit = grid[my][mx]
                break
        if not hit:
            continue
        perp = (sdx - ddx) if side == 0 else (sdy - ddy)
        perp = max(perp, 0.05)
        zbuf[x] = perp
        u = (py + perp * rdy) if side == 0 else (px + perp * rdx)
        facade = side == 1  # facades face the street (north/south)
        line = H / perp
        bottom = HORIZON + cam_h * line
        top = bottom - hit[4] * line
        fog = min(1.0, perp / 34) ** 1.1 * 0.75
        for y in range(max(0, int(top)), min(H, int(bottom) + 1)):
            v = (bottom - y) / line
            if v < 0 or v > hit[4]:
                continue
            c = wall_texel(hit, u, v, facade)
            if not facade:
                c = mul(c, 0.72)
            img[y][x] = mix(c, HAZE, fog)

    # horse sprite
    if horse_at and not mounted:
        draw_horse(img, zbuf, cam, cam_dir, (plx, ply), cam_h, horse_at)
    if mounted:
        draw_mount(img)
    return img


HORSE = [
    "..MM..................",
    ".KBBM.................",
    "KBBBBM................",
    "WBBBBBM...............",
    ".BB.BBBBBSSSBBBBBB....",
    "....BBBBBSSSBBBBBBBK..",
    "....BBBBBBBBBBBBBBBKK.",
    "....BBBBBBBBBBBBBBB..K",
    "....BBBBBBBBBBBBBBB..K",
    ".....BB.B.......B.BB..",
    ".....B..B.......B..B..",
    ".....B..B.......B..B..",
    ".....K..K.......K..K..",
]
HORSE_COL = {"B": (122, 74, 40), "K": (52, 32, 20), "M": (30, 22, 18),
             "W": (238, 232, 220), "S": (150, 36, 30)}


def draw_horse(img, zbuf, cam, cam_dir, plane, cam_h, at):
    dx, dy = cam_dir
    plx, ply = plane
    sx, sy = at[0] - cam[0], at[1] - cam[1]
    inv = 1.0 / (plx * dy - dx * ply)
    tx = inv * (dy * sx - dx * sy)
    ty = inv * (-ply * sx + plx * sy)
    if ty <= 0.1:
        return
    scr_x = W / 2 * (1 + tx / ty)
    scale = H / ty
    sw, sh = 2.0 * scale, 1.25 * scale
    bottom = HORIZON + cam_h * scale
    top = bottom - sh
    left = scr_x - sw / 2
    rows, cols = len(HORSE), len(HORSE[0])
    for x in range(max(0, int(left)), min(W, int(left + sw) + 1)):
        if ty >= zbuf[x]:
            continue
        col = int((x - left) / sw * cols)
        if not 0 <= col < cols:
            continue
        for y in range(max(0, int(top)), min(H, int(bottom))):
            row = int((y - top) / sh * rows)
            if 0 <= row < rows and HORSE[row][col] != ".":
                img[y][x] = HORSE_COL[HORSE[row][col]]


def draw_mount(img):
    base_y = H - 26
    cxm = W // 2
    for y in range(base_y, H):
        k = y - base_y
        half = 5 + k * 0.62
        for x in range(int(cxm - half), int(cxm + half) + 1):
            edge = abs(x - cxm) / half
            c = mul((122, 74, 40), 1.08 - 0.45 * edge ** 2)
            if abs(x - cxm) < 2.2 and k < 20:
                c = (34, 24, 18)  # mane
            img[y][x] = c
    for ex in (-5, 5):  # ears
        for k in range(6):
            for x in range(cxm + ex - (5 - k) // 2, cxm + ex + (5 - k) // 2 + 1):
                img[base_y - 6 + k][x] = (96, 58, 32)
    for side in (-1, 1):  # reins
        for k in range(28):
            x = int(cxm + side * (6 + k * 1.9))
            y = base_y + 6 + int(k * 0.75)
            if 0 <= x < W and y < H:
                img[y][x] = (44, 30, 20)


# ------------------------------------------------------------------ output
def write_ans(img, path):
    out = []
    for r in range(0, H, 2):
        cells = []
        for x in range(W):
            t, b = img[r][x], img[r + 1][x]
            cells.append("\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm▀" % (t + b))
        out.append("".join(cells) + "\033[0m")
    open(path, "w", encoding="utf-8", newline="\n").write("\n".join(out) + "\n")


def write_png(img, path, scale=4):
    raw = b""
    for y in range(H):
        row = b"".join(bytes(img[y][x]) * scale for x in range(W))
        raw += (b"\x00" + row) * scale
    def chunk(tag, data):
        c = tag + data
        return struct.pack(">I", len(data)) + c + struct.pack(">I", zlib.crc32(c) & 0xFFFFFFFF)
    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", W * scale, H * scale, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
    open(path, "wb").write(png)


if __name__ == "__main__":
    outdir = sys.argv[1]
    cam = (31.0, 6.0)
    look_west = (-1.0, 0.0)
    walk = render(cam, look_west, 0.5, horse_at=(27.6, 6.9), mounted=False)
    ride = render(cam, look_west, 0.72, horse_at=None, mounted=True)
    a = math.radians(-35)
    turned = render((24.0, 6.2), (math.cos(math.pi + a), math.sin(math.pi + a)), 0.5,
                    horse_at=None, mounted=False)
    write_ans(turned, outdir + "/fp_turn.ans")
    write_png(turned, outdir + "/fp_turn.png")
    write_ans(walk, outdir + "/fp_walk.ans")
    write_png(walk, outdir + "/fp_walk.png")
    write_ans(ride, outdir + "/fp_ride.ans")
    write_png(ride, outdir + "/fp_ride.png")
    print("done")
