#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifndef _WIN32
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#else
#include <windows.h>
#endif

#ifdef _WIN32
#define lstat stat
#endif

#ifndef S_ISLNK
#define S_ISLNK(mode) 0
#endif

#ifndef S_IRUSR
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100
#endif

#ifndef S_IRGRP
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010
#endif

#ifndef S_IROTH
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001
#endif

void print_permissions(mode_t mode) {
    char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';
    printf("%s ", perms);
}

void do_long_listing(const char *dir_path, int show_hidden) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_path, entry->d_name);

        if (lstat(fullpath, &file_stat) == -1) {
            perror("lstat");
            continue;
        }

        print_permissions(file_stat.st_mode);
        printf("%3ld ", (long)file_stat.st_nlink);

#ifndef _WIN32
        struct passwd *pwd = getpwuid(file_stat.st_uid);
        if (pwd != NULL) {
            printf("%-8s ", pwd->pw_name);
        } else {
            printf("%-8d ", (int)file_stat.st_uid);
        }

        struct group *grp = getgrgid(file_stat.st_gid);
        if (grp != NULL) {
            printf("%-8s ", grp->gr_name);
        } else {
            printf("%-8d ", (int)file_stat.st_gid);
        }
#else
        printf("%-8s %-8s ", "user", "group");
#endif

        printf("%8ld ", (long)file_stat.st_size);

        char timebuf[64];
        char *time_str = ctime(&file_stat.st_mtime);
        if (time_str != NULL) {
            strncpy(timebuf, time_str, sizeof(timebuf));
            timebuf[strcspn(timebuf, "\n")] = 0;
            printf("%s ", timebuf + 4);
        }

        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

void do_column_display(const char *dir_path, int show_hidden) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    int count = 0;
    int capacity = 10;
    char **files = malloc(capacity * sizeof(char *));
    size_t max_len = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_hidden && entry->d_name[0] == '.') continue;

        if (count >= capacity) {
            capacity *= 2;
            files = realloc(files, capacity * sizeof(char *));
        }

        files[count] = strdup(entry->d_name);
        size_t len = strlen(entry->d_name);
        if (len > max_len) {
            max_len = len;
        }
        count++;
    }
    closedir(dir);

    if (count == 0) {
        free(files);
        return;
    }

    int term_width = 80;
#ifndef _WIN32
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
        term_width = w.ws_col;
    }
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        term_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif

    int spacing = 2;
    int col_width = max_len + spacing;
    int num_cols = term_width / col_width;
    if (num_cols < 1) num_cols = 1;

    int num_rows = (count + num_cols - 1) / num_cols;

    for (int r = 0; r < num_rows; r++) {
        for (int c = 0; c < num_cols; c++) {
            int index = c * num_rows + r;
            if (index < count) {
                printf("%-*s", col_width, files[index]);
            }
        }
        printf("\n");
    }

    for (int i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
}

int main(int argc, char *argv[]) {
    int opt;
    int long_listing = 0;
    int show_hidden = 0;

    opterr = 0;
    optind = 1;

    while ((opt = getopt(argc, argv, "1la")) != -1) {
        switch (opt) {
            case '1':
            case 'l':
                long_listing = 1;
                break;
            case 'a':
                show_hidden = 1;
                break;
            default:
                break;
        }
    }

    if (long_listing) {
        do_long_listing(".", show_hidden);
    } else {
        do_column_display(".", show_hidden);
    }

    return 0;
}