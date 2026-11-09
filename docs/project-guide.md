# Project guide

## Academic context

The original [Operating Systems report](project-report.pdf), dated May 2025, presents three areas: file-manipulation commands in C, a command-line interpreter, and filesystem administration/analysis. The authors used Ubuntu through VirtualBox and WSL.

The runnable material in this repository is the C toolkit and interpreter. The storage section discusses a new virtual disk, partitioning, LVM physical/volume groups and logical volumes, ext3/ext4, mounting, ownership and permissions. It also documents a FAT12 image examined with Sleuth Kit tools. The corresponding VM/disk images and standalone administration scripts are absent from the supplied archive.

## Source mapping

| Original source | English source | Executable |
| --- | --- | --- |
| `mostra.c` | `src/show.c` | `bin/os-show` |
| `copia.c` | `src/copy.c` | `bin/os-copy` |
| `acrescenta.c` | `src/append.c` | `bin/os-append` |
| `apaga.c` | `src/remove.c` | `bin/os-remove` |
| `conta.c` | `src/count.c` | `bin/os-count` |
| `lista.c` | `src/list.c` | `bin/os-list` |
| `informa.c` | `src/info.c` | `bin/os-info` |
| `interpretador.c` | `src/shell.c` | `bin/os-shell` |
| `Relatorio-SO.pdf` | `docs/project-report.pdf` | Original PDF, unchanged |

The English sources are adaptations rather than byte-identical originals. Original filenames and checksums are recorded in the [manifest](source-manifest.json). The report shows the original Portuguese commands and code; its screenshots are historical examples, not output from the translated executables.

## Integration changes

- Translated source filenames, identifiers, messages and usage instructions; added a Makefile and integration checks.
- Replaced incorrect hard-coded literal lengths passed to `write` with lengths derived from the actual strings, preventing reads beyond the message.
- Added a shared I/O helper for interrupted reads and partial writes during content transfers, plus source-read failure checks in copy, append and count.
- Rejected source/destination aliases in append and copy before processing; copy checks before truncation. This prevents self-append loops and truncating a source reached through a destination alias. This check does not provide protection against concurrent filesystem races.
- Added path-truncation checks for copy and directory listing.
- Changed the copy suffix from `.copia` to `.copy` and the interpreter's quit command from `termina` to `exit`.
- Replaced `strsep` with POSIX `strtok_r`. The shell rejects excess arguments/oversized lines, retries interrupted `waitpid`, and uses child exit status 127 for a failed execution.
- Corrected the metadata label: `st_ctime` is the status-change time, not a portable creation-time field. Metadata now uses `lstat` so symbolic links can be identified.

The core design remains a set of independent programs using file descriptors, plus a synchronous fork/exec/wait interpreter. Each executable accepts the arguments listed in the README and returns a nonzero status on a detected error. Numeric error statuses are local to each utility, not a shared protocol.

## Boundaries

The shell passes whitespace-delimited tokens directly to `execvp`. It has no quote parser, expansion, pipeline or redirection support. Its `exit` built-in applies only when used as the sole token. It returns zero when the user exits normally or input reaches EOF, rather than propagating the last child status.

Newline counting counts `\n` bytes; it does not count every nonempty final text fragment as a line. Directory output is unsorted and includes dot entries. Copies use ordinary file I/O and may overwrite an existing `.copy` file; they do not preserve permissions, timestamps or sparse-file layout. Symbolic links can be followed by copy/append. Removal uses `unlink`, so it is not a recursive directory remover.

The original report is preserved as academic source material in Portuguese. This English guide summarizes scope and adaptations; it is not a full translation of the PDF.
