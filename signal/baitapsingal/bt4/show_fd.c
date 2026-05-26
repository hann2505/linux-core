#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

/* Mở một file thêm để tạo fd mới, rồi in tất cả fd của chính mình */
int main() {
    /* Mở thêm 1 file để minh họa */
    int extra_fd = open("/etc/hostname", O_RDONLY);

    printf("PID = %d\n", getpid());
    printf("File descriptors (/proc/self/fd):\n");

    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/fd", getpid());

    DIR *dir = opendir(path);
    if (!dir) { perror("opendir"); return 1; }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char fdpath[128], target[256];
        snprintf(fdpath, sizeof(fdpath), "%s/%s", path, entry->d_name);
        ssize_t len = readlink(fdpath, target, sizeof(target) - 1);
        if (len > 0) {
            target[len] = '\0';
            printf("  fd %s -> %s\n", entry->d_name, target);
        }
    }
    closedir(dir);

    if (extra_fd >= 0) close(extra_fd);
    return 0;
}
