Gungi Notation
==============

![](assets/logo.png)

# Table of Contents

- [Overview](#overview)
  - [Identifiers](#identifiers)
  - [Header](#header)
  - [Movetext](#movetext)
  - [Comments](#comments)
- [Example](#example)

# Overview

Gungi provides a notation for specifying the moves in a game.  The goal of this
notation is to make it easy to represent an entire game by specifying the moves
each player took in the game.  An optional header can also provide information
such as who the players were, the date the game was played, and if there was a
winner, a draw, or if the game is still on-going.

A file specifying this notation is called a *Gungi Notation* file and has the
file extension `.gn`.

## Identifiers

Each unit needs a unique identifier.  There are a total of `20` unique
identifiers.  Additionally, since each unit has a front and back side, except
the commander, each unit requires two characters to represent it.

| Name                | Identifier |
| ------------------- | ---------- |
| Arrow               | `A`        |
| Bow                 | `B`        |
| Bronze              | `Z`        |
| Captain             | `C`        |
| Catapult            | `T`        |
| Clandestinite       | `N`        |
| Commander           | `O`        |
| Dragon King         | `K`        |
| Fortress            | `F`        |
| Gold                | `G`        |
| Hidden Dragon       | `H`        |
| Lance               | `L`        |
| Pawn                | `P`        |
| Phoenix             | `X`        |
| Pike                | `E`        |
| Pistol              | `I`        |
| Prodigy             | `R`        |
| Samurai             | `S`        |
| Silver              | `V`        |
| Spy                 | `Y`        |

So, for example, a black unit with Spy currently front, and Clandestinite as
the back would be identified by `YN`.  If the piece has no back, then `-` is
used instead.

## Header

A header is optional in a a Gungi Notation file, but can provide additional
information not related to actual gameplay.  There are six pieces of
information that the header can provide:

  - **Event**: the name of the event or match, if any.
  - **Date**: a date string formatted as `YYYY.MM.DD`.
  - **Location**: where the event took place.
  - **White**: the name of the player who is playing as white.
  - **Black**: the name of the player who is playing as black.
  - **Result**: `1 - 0` if white won, `0 - 1` if black won, `1/2 - 1/2` if the
    game ended in a draw, or `*` if the game is still ongoing.

The format of the header is provided below:

```
[Event "Selection"]
[Date "2013.10.30"]
[Location "NGL, Mitene Union"]
[White "Komugi"]
[Black "Meruem"]
[Result "1 - 0"]
```

## Movetext

The movetext describes the actual moves that took place in the game.  Moves are
generally grouped in pairs of two, except in specific circumstances, with a
move indicator number followed by three periods if it's white's move, otherwise
one period: `1... move1 move2` indicates that `move1` is white's move, and
`move2` is black's move, while `2. move1` indicates that `move1` is black's
move.  Generally moves are specified in pairs except when dealing with a forced
recovery or forced rearrangement.

| Type                          | Format                           |
| ----------------------------- | -------------------------------- |
| Move                          | `unit<col-row-tier>col-row-tier` |
| Drop                          | `unit*col-row-tier`              |
| Mobile Strike                 | `unit<col-row-tierxcol-row-tier` |
| Immobile Strike               | `unit<col-row-tierxtier`         |
| Forced Recovery (to self)     | `unit+col-row-tier`              |
| Forced Recovery (to opponent) | `unit^col-row-tier`              |
| No Forced Recovery            | `unit=col-row-tier`              |
| Substitution                  | `unit<col-row-tier&col-row-tier` |
| 1-3 Tier Exchange             | `unit<col-row-tier&tier`         |

where `unit` is the unit identifier (e.g. `YN`), `row` is the row in `0-8`,
`col` is the column in `0-8`, `tier` is the tier in `0-2`.  Understanding the
syntax is simple: `x` indicates capturing the unit at the position that
follows, `&` indicates a substitution or exchange, `*` indicates a drop,
`<` indicates the position coming from, `>` indicates the position going to,
`+` indicates a forced recovery in which the unit is returned to the hand of
the player invoking the forced recovery, while `^` indicates that the unit
being force recovered is returned to the hand of the opposing player.  The `=`
indicates that the unit that has moved into a position where it could force
recovery has opted not to recover.

*Mobile Strike* move indicators is optional.  The Mobile Strike can be
represented instead by a Move.  Note that *No Forced Recovery* **must** be
specified in order to say that th eforced recovery will not be taking place.

## Comments

Comments can be embedded within the header or movetext of a Gungi Notation
file.  There are two types of comments: single-line comments and inline
comments.  A single line comment must come at the start of a line and begins
with a `#`.  An inline comment begins with an open parenthesis, `(`, and ends
wtih a closing parenthesis, `)`.  Both types of comments are illustrated below:

```
# This is a single-line comment.
# The game is starting.
1. PZ*0-8-0 (this is an inline comment, black is dropping their Pawn-Bronze
into column 0, row 8) PZ*0-0-0 (white responds by dropping their
Pawn-Bronze into column 0, row 0)
```

# Example

```
[Event "Selection"]
[Date "2013.10.30"]
[Location "NGL, Mitene Union"]
[White "Komugi"]
[Black "Meruem"]
[Result "*"]
# King's Selection match between Komugi and Meruem
# Witnessed by Neferpitou
1. PZ*0-8-0 PZ*0-0-0 2. PZ*8-8-0 PZ*1-1-0
```
