# midi-interval-processor

C++17 command-line tool that reads a MIDI file, computes the intervals between notes, and writes them to `<FILENAME>_intervals.txt`. Meant for studying the intervallic content of classical composers (2019 project).

## Build and test
```
make midiToInterval      # build the CLI
./midiToInterval file.mid
make test                # builds and runs the GoogleTest suite (needs gtest installed)
make clean
```
- Compiler flags: `-std=c++17 -Wall -Wextra` (set once as `CXXFLAGS` in the Makefile). Keep the code warning-free. C++17 because current GoogleTest requires it.
- Tests are in `testMidiToInterval.cpp`. They use the pieces in `testData/` plus small MIDI files built from bytes in the test itself (`midiFile`, `track`) and written to `testData/built_*`. The Dockerfile (Ubuntu 18.04 + gtest) builds the project and runs `make test`, so use it if gtest isn't available locally: `docker build ./ --tag midi_interval_tests`.
- `processMIDICorpus.sh` batch-runs the CLI over `ClassicalComposers/<composer>/*.mid` and concatenates results into `ClassicalComposers/Results/<composer>_all_intervals.txt`. That folder isn't in the repo; it must exist before running.

## Layout
- `main.cpp` — thin CLI wrapper around `run()`: exits 0 on success, 1 on an unreadable or malformed file, 2 on bad usage.
- `midiToInterval.{h,cpp}` — `run(fileName)` returns the note count and writes the intervals file, throwing `std::runtime_error` on bad input; has debug-print flags at the top.
- `noteGetter.{h,cpp}` — MIDI byte parsing, turns a file into notes.
- `musicalConcept.{h,cpp}` — abstract `musicalConcept` base with `note` and `interval` subclasses (int representation plus `toString()`). The interval is `abs` of the pitch-class difference, and all tracks are joined into one stream. Both are deliberate (a quick, uniform measure for a classifier), so don't change them without asking.

## Conventions
- Every file starts with a header block comment. Copy it from `main.cpp` into new files.
- Comment block above each function describing what it does; camelCase for functions and variables, lowercase class names.
- Adding a source file means adding its `.o` rule and linking it in both `midiToInterval` and `testMidiToInterval` in the Makefile (rules are explicit, not pattern-based).
- Tests assert on `run()`'s returned note count and on the intervals file's contents, so changing the output format or interval calculation will break them.
- Test data comes from mutopiaproject.org; keep the citations in the README when adding pieces.

## Public repo
This repo is public. Don't commit personal details (full name, email, local paths, device names), other people's contact info, or secrets. Ask first if a task needs one.

## Git workflow
- Never commit directly to `master`. Before making any commit, create a branch named for the work (e.g. `feature/add-glitch-mode`, `fix/header-parsing`, `docs/update-notes`) and commit there.
- If you're on `master` when asked to commit, switch to a new branch first.
- Keep commits small and focused, with a short imperative message.
- Pushing to a branch Claude created is fine without asking. Ask before force-pushing or pushing to any other branch (including `master`).
- Open a PR without asking once the work on a branch Claude created is done (verified, docs updated). For any other branch, ask first. Don't merge PRs.
- Before opening a PR, do a final pass over the whole diff against `master`: is it readable, well designed, and no more verbose than it needs to be? Fix what you find, without changing behaviour, then re-verify (`make test`) and note what the pass changed in the PR description.
- Stage specific files (not `git add -A`), since stray files like `.DS_Store` are lying around.
