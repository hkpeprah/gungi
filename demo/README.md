Demo Application
================

The demo application allows you to play a game of Gungi in the command-line
terminal.

# Usage

The interface for running the application from the command-line is:

```
build/demo/demo [ -h | --help ] [ -o | --output file ] [ -i | --input file]
                [ -l | --location location ] [ -e | --event event ]
                [ -w | --white name ] [ -b | --black name ]

Optional Args:
  -h\--help                 show this dialog
  -o\--output file          output '.gn' file
  -i\--input file           input '.gn' file to load
  -l\--location location    match location
  -e\--event event          name of the match
  -w\--white name           name of the whie player
  -b\--black name           name of the black player
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

# TODO

- Export moves as movetext in Gungi Notation file.
