# midi-interval-processor

C++11 command-line tool that reads a MIDI file, computes the intervals between notes, and writes them to `<FILENAME>_intervals.txt`. Meant for studying the intervallic content of classical composers (2019 project).

## Build and test
```
make midiToInterval      # build the CLI
./midiToInterval file.mid
make test                # builds and runs the GoogleTest suite (needs gtest installed)
make clean
```
- Compiler flags: `g++ -std=c++11 -Wall`. Keep the code warning-free and C++11 compatible (no C++14/17 features).
- Tests are in `testMidiToInterval.cpp` and use fixtures in `testData/`. The Dockerfile (Ubuntu 18.04 + gtest) builds the project and runs `make test`, so use it if gtest isn't available locally: `docker build ./ --tag midi_interval_tests`.
- `processMIDICorpus.sh` batch-runs the CLI over `ClassicalComposers/<composer>/*.mid` and concatenates results into `ClassicalComposers/Results/<composer>_all_intervals.txt`. That folder isn't in the repo; it must exist before running.

## Layout
- `main.cpp` — thin CLI wrapper that calls `run(argv[1])` (no argument checking).
- `midiToInterval.{h,cpp}` — `run(fileName)` returns the note count and writes the intervals file; has debug-print flags at the top.
- `noteGetter.{h,cpp}` — MIDI byte parsing, turns a file into notes.
- `musicalConcept.{h,cpp}` — abstract `musicalConcept` base with `note` and `interval` subclasses (int representation plus `toString()`).

## Conventions
- Every file starts with the header block comment ("Developed for use in midi-interval-processer / Benjamin Hasker / 2019"). Copy it into new files.
- Comment block above each function describing what it does; camelCase for functions and variables, lowercase class names.
- Adding a source file means adding its `.o` rule and linking it in both `midiToInterval` and `testMidiToInterval` in the Makefile (rules are explicit, not pattern-based).
- Tests assert on `run()`'s returned note count and on the output file, so changing the output format will break the `resultsTest` cases.
- Test data comes from mutopiaproject.org; keep the citations in the README when adding pieces.

## Git workflow
- Never commit directly to `master`. Before making any commit, create a branch named for the work (e.g. `feature/add-glitch-mode`, `fix/header-parsing`, `docs/update-notes`) and commit there.
- If you're on `master` when asked to commit, switch to a new branch first.
- Keep commits small and focused, with a short imperative message.
- Pushing to a branch Claude created is fine without asking. Ask before force-pushing or pushing to any other branch (including `master`).
- Open a PR without asking once the work on a branch Claude created is done (verified, docs updated). For any other branch, ask first. Don't merge PRs.
- Before opening a PR, do a final pass over the whole diff against `master`: is it readable, well designed, and no more verbose than it needs to be? Fix what you find, without changing behaviour, then re-verify (`make test`) and note what the pass changed in the PR description.
- Stage specific files (not `git add -A`), since stray files like `.DS_Store` are lying around.

<!-- claude-harness:start -->
## Guardrails
- Done means `.claude/verify.sh` passes. A Stop hook runs it when you finish a turn with work on the branch and sends you back to fix failures. If it can't pass (missing tool, flaky test), tell the user why instead of weakening the script.
- `.claude/hooks/block-dangerous.sh` blocks force pushes, pushes to main/master, deleting main/master, `git reset --hard origin/...`, recursive `rm` outside the repo, reading `.env`/key/credential files, and `curl | sh`. If you need something it blocks, ask the user. Don't work around it or edit the hooks or `.claude/settings.json` to get past it.
- Shell commands run in a sandbox: they can write only inside the repo and temp directories, and reach only the allowed hosts. `git push` and `gh pr ...` run outside it, but only on their own, not chained with `&&`. Other `gh` commands fail inside it. If the sandbox blocks something you need, tell the user rather than working around it.
- Always work on a branch and finish with a PR; never commit or push to main/master. Commit in small checkpoints as you go.
- Ask before adding a new dependency.
<!-- claude-harness:end -->
