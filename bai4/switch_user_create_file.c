#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>

int main() {
    const char *targetUser = "userB";

    // Lấy thông tin userB
    struct passwd *pw = getpwnam(targetUser);

    if (pw == NULL) {
        perror("getpwnam");
        return 1;
    }

    uid_t target_uid = pw->pw_uid;
    gid_t target_gid = pw->pw_gid;

    printf("Current UID before switch: %d\n", getuid());
    printf("Switching to %s (UID=%d)\n", targetUser, target_uid);

    // Đổi group trước
    if (setgid(target_gid) != 0) {
        perror("setgid");
        return 1;
    }

    // Đổi user
    if (setuid(target_uid) != 0) {
        perror("setuid");
        return 1;
    }

    printf("Current UID after switch: %d\n", getuid());

    // Tạo file
    int fd = open(
        "/tmp/output_userB.txt",
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *msg = "File created by userB\n";

    write(fd, msg, strlen(msg));

    close(fd);

    printf("File created successfully.\n");

    return 0;
}
