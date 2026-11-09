# POSIX File Toolkit

A small **C project for Unix file operations and process management**: seven file utilities and a command interpreter built around POSIX system calls.

Developed as a group project for **Operating Systems** at **IPCA**, submitted in **May 2025**. The project explores file descriptors, directory traversal, file metadata, and the `fork` → `execvp` → `waitpid` process lifecycle.

## Commands

| Executable | Usage | Behavior |
| --- | --- | --- |
| `os-show` | `os-show <file>` | Write a file's bytes to standard output |
| `os-copy` | `os-copy <file>` | Copy to `<file>.copy`, replacing an existing distinct destination |
| `os-append` | `os-append <source> <destination>` | Append source bytes to an existing destination |
| `os-remove` | `os-remove <file>` | Remove a file's directory entry using `unlink` |
| `os-count` | `os-count <file>` | Count newline characters (`\n`) |
| `os-list` | `os-list [directory]` | List regular files and directories; default directory is `.` |
| `os-info` | `os-info <file>` | Display type, inode, owner UID and timestamps |
| `os-shell` | `os-shell` | Run commands sequentially and report child exit status; `exit` quits |

Names use the `os-` prefix to distinguish these educational utilities from system commands. Source filenames, identifiers and console messages are in English.

## Build and test

Requirements: a POSIX environment, a C compiler and Make. Linux or WSL is recommended. The code uses C11 syntax and POSIX.1-2008 APIs; it is not a native Windows console application. Python 3 is needed only for the integration tests.

```sh
make
make test
```

Executables are generated in `bin/`. The default flags enable `-std=c11 -Wall -Wextra -Wpedantic` and expose POSIX APIs through `_POSIX_C_SOURCE=200809L`.

```sh
make clean
```

This removes the generated `bin/` directory. No administrator permissions are required to build or run the examples.

## Try it

From the repository root, create a disposable example and inspect it:

```sh
printf 'Operating Systems\nFile Toolkit\n' > sample.txt
./bin/os-show sample.txt
./bin/os-count sample.txt
./bin/os-copy sample.txt
./bin/os-info sample.txt
./bin/os-list .
```

`os-count` prints `Newline count: 2`; the copy is saved as `sample.txt.copy`.

```sh
./bin/os-append sample.txt sample.txt.copy
./bin/os-show sample.txt.copy
./bin/os-remove sample.txt.copy
```

The copy now contains the original text twice before it is removed. `os-copy` overwrites an existing distinct `.copy` file, `os-append` modifies its destination, and `os-remove` unlinks its target; use disposable files for experiments.

## Command interpreter

Start it from the repository root:

```sh
./bin/os-shell
```

At its `%` prompt, try:

```text
./bin/os-show sample.txt
./bin/os-count sample.txt
./bin/os-info sample.txt
exit
```

The interpreter splits a line on whitespace, starts a child with `fork`, replaces that child with the requested program using `execvp`, and waits using `waitpid`. It prints whether the command exited normally and its exit code. Program lookup follows `PATH`; explicit paths such as `./bin/os-show` also work.

This is a sequential command interpreter. It does not implement quoting/escaped spaces, pipes, redirection, variable expansion, globbing, job control, or built-ins such as `cd`. A line can contain up to 1022 bytes before its newline and at most 63 tokens, including the executable name. Oversized lines and excess arguments are rejected rather than partially executed.

## Implementation

| Area | Main APIs | Source |
| --- | --- | --- |
| File contents | `open`, `read`, `write`, `close` | [show.c](src/show.c), [copy.c](src/copy.c), [append.c](src/append.c), [count.c](src/count.c) |
| File removal | `unlink` | [remove.c](src/remove.c) |
| Directory traversal | `opendir`, `readdir`, `stat`, `closedir` | [list.c](src/list.c) |
| Metadata | `lstat`, `localtime`, `strftime` | [info.c](src/info.c) |
| Process lifecycle | `fork`, `execvp`, `waitpid` | [shell.c](src/shell.c) |
| Interrupted/partial I/O | Retry interrupted reads and complete short writes | [io.h](src/io.h) |

`os-info` uses `lstat`, so it describes a symbolic link itself. Its **Status changed** field is `st_ctime`, the inode status-change time, not file creation time. `os-list` uses `stat`, follows symbolic links to their targets, includes `.` and `..`, and prints entries in filesystem order.

## Documentation and academic scope

- [Project report](docs/project-report.pdf): original group report, unchanged and in Portuguese.
- [Project guide](docs/project-guide.md): English overview, source-name mapping, behavior changes and limitations.
- [Source manifest](docs/source-manifest.json): original archive filenames and SHA-256 checksums.
- [Integration tests](tests/test_commands.py): checks using disposable temporary files.

The report also describes VirtualBox storage setup, LVM, ext3/ext4 filesystems, permissions and FAT12 analysis with Sleuth Kit. The ZIP contains the eight C sources and report. **VM images, a FAT12 disk image, standalone storage scripts and an assignment brief were not supplied.** Those report sections are retained as academic documentation, not presented as runnable labs in this repository.

## Validation and limitations

The eight executables compiled without compiler warnings and passed the integration tests on Linux. Tests cover byte-for-byte copying across multiple buffers, replacement of an existing copy, newline counting, append-to-self and aliased-copy rejection, metadata, symbolic links, file removal, missing inputs and shell child execution/failure handling.

This is an educational toolkit, not a complete replacement for Unix utilities. File copies do not preserve metadata or provide atomic replacement. Paths are limited to 255 bytes for the generated copy destination and 511 bytes for directory-listing paths. Copying and appending are intended for ordinary files; concurrent modification and filesystem races are not addressed. Counting uses an integer counter, and a final unterminated text line adds no newline to the count. Diagnostic/metadata writes do not implement the same full retry handling as file-content transfers.

## Authors

- **Ana Rita Ranito** — a31511
- **Érica Segredo** — a30623
- **Filipe Fernandes** — a31498
- **Ruben Costa** — a23508

Operating Systems · Instituto Politécnico do Cávado e do Ave (IPCA) · May 2025. Course instructor: Nuno Lopes.

## License

No license has been selected. The original group authorship is retained; no separate reuse license is granted.
