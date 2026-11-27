# World files

Everything about the outdoor 3D town lives in this folder as plain text. Edit a
file, save, and run `./playGame` again: there is nothing to rebuild.

If a line has a mistake, the game skips that line and prints a warning naming
the file and line number when it starts. If `town.map` is missing, a small
built-in street is used instead, so the game always starts.

Coordinates are in **tiles**. `x` grows to the east (right on the map), `y`
grows to the south (down the map). The top-left tile is `0,0`. A tile's centre
is at `x + 0.5, y + 0.5`.

| File | What it controls |
|---|---|
| `town.map` | The map: ground, rock, fences, building footprints, the start |
| `buildings.txt` | Each building's name, sign, height, wall style, colours, door |
| `zones.txt` | Which area counts as which location, and what `look` says there |
| `props.txt` | Things standing in the world: headstones, cacti, barrels, the wagon |
| `sprites/*.txt` | The pixel art for those things, and for your horse |
| `sky.txt` | Sky colour and light through the day and night |

---

## `town.map`

One character per tile. Rows can be different lengths; short rows are padded
with sand.

| Char | Tile | Blocks you? |
|---|---|---|
| `.` | sand | no |
| `=` | street (packed dirt) | no |
| `_` | boardwalk | no |
| `,` | dry grass | no |
| `~` | salt flat (the lakebed) | no |
| `@` | where you start (sand underneath) | no |
| `#` | rock (the hills) | yes, drawn as a 2.6-tile cliff |
| `F` | picket fence | yes, drawn low |
| `a`-`z` | a building's footprint, defined in `buildings.txt` | yes |

A lower-case letter that isn't defined in `buildings.txt` is treated as open
sand. Anything else unknown is also sand. The edge of the map stops you, but
the desert is drawn on to the horizon.

**Example:** to add a shack, draw its footprint with an unused letter:

```
....qqq....
....qqq....
```

then add a line for `q` in `buildings.txt`.

---

## `buildings.txt`

```
letter id "SIGN" height style base_rgb trim_rgb door_x,door_y facing room_id
a hotel "HOTEL" 1.9 planks 150,88,62 235,220,190 6,15 S hotel_lobby
```

| Field | Meaning |
|---|---|
| `letter` | The map letter for this building's footprint |
| `id` | A short name, e.g. `saloon`. `enter saloon` matches it |
| `"SIGN"` | The sign text, in quotes (spaces allowed). `enter general store` matches it too |
| `height` | Wall height in tiles; `1.0` is one storey. Windows: one row from 1.0, two rows from about 1.9 |
| `style` | `planks`, `adobe`, `barn`, `church`, `saloon`, `mine` or `well` |
| `base_rgb` | Main wall colour, `red,green,blue`, each 0-255 |
| `trim_rgb` | Window frames, door frame and roofline |
| `door_x,door_y` | The tile **just outside** the door: stand within 1.5 tiles of it and type `enter` |
| `facing` | Which way the front wall faces: `N`, `S`, `E` or `W`. Door, windows and sign go on that wall. When you `leave`, you come out facing this way |
| `room_id` | The location `enter` opens (a room id from the game, e.g. `saloon`, `hotel_lobby`) |

---

## `zones.txt`

```
room_id x0 y0 x1 y1 description shown by "look"
graveyard 24 21 35 28 Wooden crosses lean in the dry grass. Some of the graves are fresh.
```

Each zone is a rectangle of tiles, from `x0,y0` to `x1,y1` inclusive. Where
zones overlap, **the one listed first wins**, so put small zones above big
ones. While you stand in a zone, the status line shows that location's name
and typed commands (`take`, `drop`, ...) act on that room.

---

## `props.txt`

```
sprite_name x y blocking
cactus 44.0 12.0 yes
```

`sprite_name` is a file in `sprites/` without `.txt`. `x y` may have
decimals: `26.5 23.5` is the centre of tile 26,23. `yes` makes the prop stop
you (within 0.3 tiles); `no` lets you walk through it. A prop whose sprite file
is missing is simply not drawn.

---

## `sprites/<name>.txt`

```
# comments start with #
size 0.7 1.1          <- width and height in tiles (how big it looks in the world)
G = 70 120 60         <- palette: one letter = one colour (red green blue)
D = 40 80 40
....GG....            <- pixel rows, top to bottom
...GGDG...
```

- `.` is see-through. So is any letter missing from the palette.
- Short rows are padded with `.`.
- The sprite stands with its bottom row on the ground.
- More rows and columns give more detail in the same `size`.
- `horse.txt` is your horse. It is drawn whenever you are on foot.

---

## `sky.txt`

```
HH:MM  sky_top_rgb  horizon_rgb  light
17:30 38,62,128 244,176,112 0.95
```

Keyframes for the time of day. Colours and light blend smoothly between them,
wrapping around midnight. `horizon_rgb` is also the colour of distance haze.
`light` runs from 0 (black) to 1 (full daylight). Below 0.45, windows glow with
lamplight. Below 0.6, stars start to come out. The sun crosses the sky from
06:00 to 19:00; the moon crosses it overnight.

The game starts at 17:30. A step costs 2 minutes, an action (`take`, `enter`,
`mount`, ...) costs 5, and `wait` costs 60. Turning, `look`, `help` and
`inventory` are free.
