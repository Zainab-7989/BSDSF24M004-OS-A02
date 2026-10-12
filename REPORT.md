# OS Assignment - Custom `ls` Utility Report

## Feature 2: Long Listing (`-l`) & Permissions
- **Difference between `stat()` and `lstat()`:** 
  `stat()` follows symbolic links and returns the metadata of the target file it points to, whereas `lstat()` examines the link itself without following it. In an `ls` command, `lstat()` is more appropriate because if an entry is a symbolic link, we want to display the link's own attributes and name rather than the target file's.
- **Using Bitwise Operators & Macros with `st_mode`:** 
  The `st_mode` field is a bitmask integer combining file type and permissions. We extract specific bits using bitwise AND (`&`) combined with predefined macros. For example, `(mode & S_IRUSR)` checks if the owner read permission bit is set, and `S_ISDIR(mode)` checks the file type bits to see if it's a directory.

---

## Feature 3: Column Display & Terminal Width (`ioctl`)
- **"Down then Across" Format & Loop Limitation:** 
  In a "down then across" format, items are filled vertically down columns before moving to the next horizontal column. A simple single loop is insufficient because items must be printed row-by-row (interleaving items from different columns), requiring prior knowledge of the total item count, row count, and column width to calculate exact indices.
- **Purpose of `ioctl` & Fixed-Width Limitations:** 
  The `ioctl` system call with `TIOCGWINSZ` queries the terminal driver at runtime to get the exact current width (columns) of the user's terminal window. If we only used a fixed 80-column fallback, the output would either wrap incorrectly on wider modern screens or break on smaller resized terminal windows, lacking responsiveness.

---

## Feature 4: Horizontal Column Display (`-x`)
- **Implementation Complexity (Vertical vs. Horizontal):** 
  The "down then across" (vertical) layout is more complex because it requires calculating row-column mathematical coordinate mappings ($index = c \times rows + r$) to fetch elements out of order. Horizontal printing is simpler and more straightforward because it just prints items sequentially from left to right, wrapping to a new line whenever the cumulative width exceeds the terminal limit.
- **Managing Display Modes:** 
  Display modes are managed using an integer flag variable (`display_mode`) set via `getopt()` argument parsing (`-l`, `-x`). The `main` function evaluates this flag in a conditional statement and delegates control by passing the file list to the corresponding dedicated function (`do_long_listing`, `do_horizontal_display`, or `do_column_display`).

---

## Feature 5: Alphabetical Sorting (`qsort`)
- **Reading into Memory & Drawbacks:** 
  It is necessary to read all directory entries into memory first because sorting requires random access to all items simultaneously before any layout or printing can begin. For directories containing millions of files, this approach consumes massive amounts of RAM and can cause performance bottlenecks or out-of-memory crashes.
- **`qsort()` Comparison Function:** 
  The `qsort()` function requires a comparison function signature taking two `const void *` pointers. It casts them into string pointer types (`char **`) and uses `strcmp()` to compare their ASCII values. It takes `void *` pointers to remain generic and capable of sorting any data type in C.

---

## Feature 6: Colorized Output Based on File Type
- **ANSI Escape Codes & Green Text:** 
  ANSI escape codes are special ASCII character sequences interpreted by the terminal emulator to alter text formatting and colors without printing characters. To print text in green, you output the sequence `\033[0;32m`, followed by your text, and then reset it using `\033[0m`.
- **Checking Executable Permission Bits:** 
  To determine if a file is executable, you inspect the permission bits within `st_mode` using bitwise AND against owner (`S_IXUSR`), group (`S_IXGRP`), and others (`S_IXOTH`) execution macros: `(file_stat.st_mode & S_IXUSR || file_stat.st_mode & S_IXGRP || file_stat.st_mode & S_IXOTH)`.

---

## Feature 7: Recursive Listing (`-R`)
- **Base Case in Recursion:** 
  A "base case" is the condition under which a recursive function stops calling itself and returns. In a recursive `ls -R`, the base case is reached when an entry is a regular file, a symlink, or when a directory contains no further subdirectories (or only `.` and `..`), preventing infinite recursive loops.
- **Essential Full Path Construction:** 
  It is essential to construct a full path (`parent_dir/subdir`) because the operating system's working directory does not change during traversal. If you simply called `do_ls("subdir")` from inside the parent directory, the function would look for that subdirectory in the program's root execution path rather than inside the nested parent folder, resulting in file-not-found errors.