Demo Application
================

The demo application allows you to play a game of Gungi in the command-line
terminal.

# Usage

The interface for running the application from the command-line is:

```
build/demo/demo [ optional arguments ]

Optional Args:
  -h, --help                          show this dialog
  -o FILE, --output FILE              output '.gn' file
  -i FILE, --input FILE               input '.gn' file to load
  -l LOCATION, --location LOCATION    match location
  -e EVENT, --event EVENT             name of the match
  -w NAME, --white NAME               name of the whie player
  -b NAME, --black NAME               name of the black player
```

# Application Commands

The commands for making moves within the game are as follows:

| Type             | Format                           |
| ---------------- | -------------------------------- |
| Drop             | `d <piece> <posn>`               |
| Move             | `m <posn>-<tier> <posn>`         |
| Immobile Strike  | `i <posn>-<tier> <tier>`         |
| Force Recover    | `fr`                             |
| No Force Recover | `nfr`                            |
| Substitution     | `s <posn>-<tier> <posn>-<tier>`  |
| Tier Exchange    | `t <posn>-<tier> <tier>`         |

# Todo

- Handle input files for move string exporting.
