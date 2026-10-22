# Protocol

Everything on the wire is text. Each number is one ASCII digit (`'0' + value`), which is why
positions stay below ten per axis and stats are scaled before sending (speed is divided by 10
or by 50 depending on the message). A buffer may carry several messages back to back: the
client parser reads the prefix, consumes the fixed or delimited body, and recurses on the rest.

## Client to server

The client sends at most one character per key press, read by the server with `atoi`.

| Sent | Meaning |
|------|---------|
| `1`, `2`, `3`, `4` | Move up, down, right, left |
| `5` | Plant a bomb |
| `0\n` | Quit (sent on Escape before the socket is closed) |

## Server to client

| Prefix | Body | Meaning |
|--------|------|---------|
| none | `p a b c d m` | Handshake: player number, four presence flags, map number (16 bytes) |
| `!` | `d p [i t v]` | Player `p` moved in direction `d`; optionally picked bonus slot `i` of type `t`, new value `v` |
| `$` | `5 p n` | Player `p` planted bomb number `n` |
| `%` | `a b c d` | Presence flags of the four players |
| `'` | 4 x `x y l s b n` | Position and stats of each player (`pppppp` for an empty slot) |
| `"` | `l1 l2 l3 l4 p n` + pairs + `$` + bonuses | Bomb `n` of player `p` exploded: lives left, then the `y x` pairs of broken tiles, then the dropped bonuses |
| `#` | `l1 l2 l3 l4 p n` | Lives after a player walked into flames that are still burning |
| `(` | `3 y x` repeated | Breakable tiles of the map (handshake) |
| `)` | `i t x y` repeated | Bonuses on the ground: slot, type, tile (handshake and explosions) |
| `*` | 4 x `i l s b n r` | Every player's stats: lives, speed, bombs, bombs at a time, radius (handshake) |

Moves are validated on the server (`gameMovePlayer_server`): a move is only broadcast when the
target tile is walkable, free of players and bombs, and the player's step delay has elapsed.
The server writes commands with `send(..., MSG_NOSIGNAL)` so a closed client does not kill the
process.
