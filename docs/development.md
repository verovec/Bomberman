# Development

## Toolchain

- Linux, `gcc` and `make`.
- SDL2 with the image and ttf extensions: `libsdl2-dev`, `libsdl2-image-dev`, `libsdl2-ttf-dev`
  on Debian and Ubuntu (`SDL2`, `SDL2_image`, `SDL2_ttf` on other distributions).
- POSIX sockets and `pthread`, provided by the system.

## Recipes

```sh
make               # compile src/*.c and link ./bomberman
make re            # clean and rebuild from scratch
make clean         # remove the object files
make fclean        # remove the object files and the binary
./bomberman        # run from the repository root, assets are loaded by relative path
```

There is no test suite. The compiler flags `-Wall -Wextra -Werror` are only applied at link
time: the object files are compiled by the generic `%.o : %.c` rule without `CFLAGS`.

## Running a game

1. Start a first `./bomberman`, select **Create game** with the arrow keys, press Enter.
2. Type a port or press Enter for `5000`; type an IP or press Enter for `127.0.0.1`.
   The window switches to the "server running" screen; Escape stops the server.
3. Start a second `./bomberman`, select **Join game**, enter the same port and IP.
4. Repeat for up to four players. Moves are accepted once two players are connected.

Both server and client print what they send and receive on standard output, which is the
easiest way to follow the protocol described in [protocol.md](protocol.md).

### On a LAN

The server binds to the IP typed in the create screen, not to every interface. Give the host's
LAN address (for example `192.168.1.10`) on the host, and the same address on each client.

## Known limits

- The host only serves: it does not get a player in its own window.
- Everything is sent as one ASCII digit per value, so the arena cannot be wider than ten tiles
  per axis in messages, and bonuses are capped to single digits.
- A player who disconnects frees their slot on the server, but the other clients are not told:
  the sprite stays on their screens (the presence broadcast is commented out in `server.c`).
- Input is validated loosely: a malformed IP is reported in the terminal and the menu is shown
  again.
