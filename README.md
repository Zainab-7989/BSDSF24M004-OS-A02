# BSDSF24M004-OS-A02

# Custom `ls` Utility in C (OS Assignment)

A feature-rich, command-line custom implementation of the standard Unix `ls` utility written in C, built and tested for Windows (MinGW/MSYS2) and Linux environments using Git Bash and manual compilation (`gcc`).

---

## Project Overview & Objectives
The primary objective of this project is to build a robust file and directory listing utility from scratch, deepening our understanding of operating system concepts such as process execution, file metadata, system calls (`stat`, `lstat`, `ioctl`), dynamic memory management, sorting algorithms, recursion, and ANSI terminal control.

---

## Key Features & Version Breakdown

### 1. Version 1.1.0: Long Listing & Argument Parsing (`-1`, `-l`)
- Implemented `getopt()` to parse basic flags.
- Added support for single-column (`-1`) and detailed long-listing (`-l`) formats.
- Extracted file metadata (`st_mode`, `st_nlink`, `st_size`, `st_mtime`) using `lstat()` and `stat()`.
- Implemented custom permission string formatting (`rwx` attributes) and timestamp parsing via `ctime()`.

### 2. Version 1.2.0: Dynamic Column Display & Terminal Width (`ioctl`)
- Replaced default single-column output with a responsive "down then across" columnar layout.
- Dynamically read directory entries into a dynamically allocated array of strings (`malloc`/`realloc`).
- Queried terminal width dynamically using the `ioctl` system call with `TIOCGWINSZ` (with a robust fallback for Windows/MinGW).
- Calculated column widths based on the length of the longest filename plus spacing.

### 3. Version 1.3.0: Horizontal Column Display (`-x`)
- Extended argument parsing to support the `-x` flag.
- Implemented row-major (left-to-right) horizontal printing logic.
- Added automatic line-wrapping (`\n`) when printing filenames exceeds the detected terminal width.

### 4. Version 1.4.0: Alphabetical Sorting (`qsort`)
- Unified directory entry collection across all modes.
- Integrated the standard C library function `qsort()` along with a custom string comparison helper function (`strcmp`) to ensure directory contents are always displayed in strict alphabetical order.
- Ensured proper memory allocation and cleanup (`free`) to avoid memory leaks.

### 5. Version 1.5.0: Colorized Output Based on File Type
- Introduced ANSI escape codes to enhance terminal readability and user experience.
- Implemented metadata checks to dynamically color-code different file types:
  - **Directories:** Blue (`\033[0;34m`)
  - **Executables:** Green (`\033[0;32m`)
  - **Archives/Tarballs (.tar, .gz, .zip):** Red (`\033[0;31m`)
  - **Symbolic Links:** Magenta (`\033[0;35m`)
  - **Special Files/Sockets:** Reverse Video (`\033[0;7m`)

### 6. Version 1.6.0: Recursive Directory Listing (`-R`)
- Implemented recursive traversal triggered by the `-R` option.
- Dynamically constructed full paths (`parent_dir/subdir`) for nested folders.
- Used `S_ISDIR` checks to recursively descend into subdirectories while printing standard directory headers (`dirname:\n`).

---

## Compilation and Execution

To compile and run the project manually using `gcc`:

```bash
# Compile the source code
gcc -Wall -g src/ls-v1.0.0.c -o bin/ls

# Run different modes
./bin/ls          # Default columnar sorted view with colors
./bin/ls -l       # Long listing format
./bin/ls -a       # Show hidden files
./bin/ls -x       # Horizontal column view
./bin/ls -R       # Recursive directory listing