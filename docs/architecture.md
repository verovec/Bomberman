# Architecture

Bomberman is one C program, `bomberman`, built from every file in `src/` with a single header.
It uses SDL2 for the window, SDL2_image for the sprites, SDL2_ttf for the HUD text, POSIX TCP
sockets for the network and `pthread` for the server. The same binary is the server and the
client: the menu decides which role the process takes.

## Files

```
Makefile              gcc, -Wall -Wextra -Werror at link time, -lSDL2 -lSDL2_image -lSDL2_ttf, -pthread
headers/main.h        every struct (gameStruct, gamePlayer, caseMap, gameMenu...) and every prototype
src/main.c            the menu loop, the port and IP inputs, the switch to server or client
src/init.c            gameInit: SDL, window 940x528, textures, players, map, bonuses, buttons, HUD
src/draw.c            menuDraw, gameDraw (map, bonuses, bombs, flames, players, HUD), waiting screen
src/events.c          menuEvent (arrows, Enter, Escape), menuTextInputEvent, text to texture helpers
src/server.c          theServer (bind, listen, accept) and connectionHandler (one thread per client)
src/events_server.c   the rules: moves, bombs, explosions, bonuses, and the messages that report them
src/client.c          theClient: connect, handshake, then the draw / send / receive / animate loop
src/events_client.c   keys to commands, and the parser that applies server messages to the local state
src/animations.c      sprite sheet stepping for walking, bomb ticking, flames and bonus glow
src/game.c            gameDestroy and the text rendering helpers
images/               menu, map tiles, characters, flames, bonuses, HUD, error banners
fonts/font.ttf        the HUD and input font
```

## Game loop

`main()` draws the menu and polls events until Enter is pressed on one of the two buttons.
Both paths then read a port (digits only, default `5000`) and an IP (digits and dots, default
`127.0.0.1`) through `SDL_StartTextInput`.

- **Create game** spawns `theServer` in a thread, builds a fresh random map with
  `mapInitServer`, then loops on `menuDraw_serverRunning` and `eventInsideServer` until Escape
  or the window closes. The host does not play: it only shows the "server running" screen.
- **Join game** calls `theClient`, which connects, reads the handshake and runs the game loop in
  the main thread: `gameDraw`, `gameEventClient_sendMessage`, a non-blocking `recv`,
  `gameEventClient_recieveMessage`, then `animations` and `bonusAnimations`.

When either path ends, the menu loop starts again.

## Server

`theServer` binds a `SOCK_STREAM` socket to the IP and port typed in the menu, listens, and
accepts connections while fewer than four clients are present. Each accepted socket takes the
first free slot in `clientSockets[4]`; the slot index is the player number. A
`connectionHandler` thread is started per client:

1. It sends the 16-byte handshake: player number, the four presence flags and the map number.
2. It sends the full state in 512 bytes (`concatenateGameStat`): breakable blocks, bonuses on
   the ground, who is online with positions, and every player's stats.
3. It then loops on a non-blocking `recv` of at most 16 bytes. Each received command is turned
   into an integer and handed to `eventServer`, which applies the rule and broadcasts the
   result to every connected socket. On each pass it also calls `bombeExplosionDuration` for
   that player, which detonates bombs older than 4 s and reports further damage during the
   following second.

A disconnected client frees its slot, so a new player can take the same number later.

## Client

`theClient` keeps a local copy of the map, the players and the bonuses, updated only by server
messages. Keys are sent as single characters and the local player does not move until the
server confirms the move, so every window shows the same positions. Moves received from the
server are queued in `animationsArray` (16 slots) and played by `animations`: a walk takes
eight sprite frames, a bomb tick cycles its sheet for about three seconds, a flame stays for
sixteen ticks of 50 ms. Keys are ignored while fewer than two players are online.

## State

| Structure | Holds |
|-----------|-------|
| `gameStruct` | window, renderer, screen sizes, the four players, the map, the menu, the sockets, the bonuses, the HUD |
| `gamePlayer` | textures per direction, pixel and tile position, `gameBombePlayer`, `gameStatsPlayer`, facing |
| `gameStatsPlayer` | lives, speed (ms per step), bombs left, bombs allowed at once, flame radius |
| `gameBombePlayer` | up to four bombs: position, sprite frame, `activated`, `exploded`, detonation time |
| `caseMap` | tile type and whether a player, a bomb or a bonus (index) is on it |
| `gameSockets` | the four client sockets (`-1` when free) and the connection count |
| `gameMenu` | fonts, buttons, input and error textures, the selected entry |

## Map

The arena is `numberOfBlocks_X` by `numberOfBlocks_Y`, 15 by 11 tiles of 48 by 48 pixels in a
720 by 528 playfield, with a 220 pixel HUD column on the right. Tile types:

| Type | Texture | Meaning |
|------|---------|---------|
| 0 | `map/0/green_1.png` | Grass, walkable |
| 1 | `map/0/green_2.png` | Grass, walkable |
| 2 | `map/0/destructible.png` | Breakable block, stops flames, may hide a bonus |
| 3 | `map/0/solid.png` | Solid pillar on every even row and column |
| 4 | `map/0/solid_exterior.png` | Outer wall |

`mapInitServer` draws the random layout: inner tiles that are not pillars become breakable
three times out of four, and the tiles around the four corners are kept clear. Clients start
from an empty layout (`mapInitClient`) and receive the breakable blocks in the handshake.
Players spawn at (1,1), (13,9), (13,1) and (1,9) in tile coordinates.

## Bombs and bonuses

A bomb can be planted on a free tile, at most once every 200 ms, while the player has bombs
left and a free bomb slot. It explodes 4 s later: flames go in the four directions up to the
player's radius, stop at a solid tile, break the first breakable tile they reach and remove a
life from every player on the way. Breaking a block drops a bonus with probability 19/30:

| Type | Sprite | Effect | Cap |
|------|--------|--------|-----|
| 0 | `bonus_1.png` | +5 bombs | 20 |
| 1 | `bonus_2.png` | 50 ms faster per step | 150 ms |
| 2 | `bonus_3.png` | +2 flame radius | 9 |
| 3 | `bonus_4.png` | +1 life | 2 |
| 4 | `bonus_5.png` | +1 bomb at a time | 4 |

Walking onto a bonus applies it on the server and the result is appended to the move message
so every client updates that player's HUD.
