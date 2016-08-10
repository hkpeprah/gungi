# Contributing to Gungi
Looking to contribute something to Gungi?  Great!  Here is how you can
help out.  But first, please take a moment to review this documentation
in order to make sure things go smoothly.

## Using the Issue Tracker
Use the Issue Tracker to file bug reports, request features, and submit
pull requests.  While all contributions are welcome, please do not use
the issue tracker for any of the following:

- Discussing ways to win in the game.
- Discussing games you've played unless talking about breaking
  behaviour, or proposing changes.

## Bug Reports
A good bug report involves a *demonstrable* problem in the existing
code with steps to reproduce it.  In general, include the following:

- What was your environment (e.g. operating system).
- What was the expected behaviour vs. what was the actual behaviour.
- Steps to reproduce.
- Any other information you feel is relevant (e.g. where you think the
  bug might lie).

## Feature Requests
Feature requests are always welcome, but please detail your feature
request in full, and how it would fit into the current project.
However, if your feature request involves new units and/or modifying
the existing units, you will have to make a pretty compelling
argument for the addition.

## Pull Requests
Pull requests are always welcome, but please ask first before embarking
on any significant pull request (e.g. implementing features,
refactoring code, porting to a different language), otherwise you risk
spending a lot of time working on something that the project's
developers might not want to merge into the project.

Please adhere to the coding conventions used throughout a project
(indentation, accurate comments, etc.) and any other requirements
(such as test coverage).

Adhering to the following process is the best way to get your work
included in the project:

1. [Fork](https://help.github.com/articles/fork-a-repo/) the project,
   clone your fork, and configure the remotes:

   ```bash
   # Clone your fork of the repo into the current directory
   git clone git@github.com:<your-username>/gungi
   # Navigate to the newly cloned directory
   cd gungi
   # Assign the original repo to a remote called "upstream"
   git remote add upstream git@github.com:hkpeprah/gungi
   ```

2. If you cloned a while ago, get the latest changes from upstream:

   ```bash
   git checkout master
   git pull upstream master
   ```

3. Create a new branch (off the main project development branch) to
   contain your feature, change, or fix:

   ```bash
   git checkout -b <your-username>/<type>/<name>
   ```

   Where `type` is the type of change being made (e.g. `feature`,
   `bugfix`, `hotfix`, etc.).  The `name` should be logical enough
   that someone can understand what the branch is implementing from
   it.

4. Locally merge (or rebase) the upstream development branch into
   your topic branch:

   ```bash
   git pull [--rebase] upstream master
   ```

5. Push your branch up to your fork:

   ```bash
   git push origin <branch-name>
   ```

7. [Open a Pull Request](https://help.github.com/articles/using-pull-requests/) with a clear title and description.

## License
By contributing your code, you agree to license your contribution under
the [MIT LICENSE](./LICENSE.md).
