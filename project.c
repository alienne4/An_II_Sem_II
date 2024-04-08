#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

// Function to get file permissions in string format
char *get_permissions(mode_t mode) {
    static char permissions[11];
    permissions[0] = (S_ISDIR(mode)) ? 'd' : '-';
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}

// Function to get owner name for a given UID
char *get_owner(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return (pw != NULL) ? pw->pw_name : "Unknown";
}

// Function to get group name for a given GID
char *get_group(gid_t gid) {
    struct group *grp = getgrgid(gid);
    return (grp != NULL) ? grp->gr_name : "Unknown";
}

// Function to format time in string format
char *format_time(time_t time) {
    static char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
    return buffer;
}

// Function to recursively traverse directories and create snapshot
void traverse_directory(const char *dirname, FILE *snapshot_file) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if ((dir = opendir(dirname)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        sprintf(path, "%s/%s", dirname, entry->d_name);

        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            traverse_directory(path, snapshot_file);
        }

        fprintf(snapshot_file, "File: %s\n", path);
        fprintf(snapshot_file, "Permissions: %s\n", get_permissions(statbuf.st_mode));
        fprintf(snapshot_file, "Owner: %s\n", get_owner(statbuf.st_uid));
        fprintf(snapshot_file, "Group: %s\n", get_group(statbuf.st_gid));
        fprintf(snapshot_file, "Size: %ld bytes\n", statbuf.st_size);
        fprintf(snapshot_file, "Last Modified: %s\n\n", format_time(statbuf.st_mtime));
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dirname = argv[1];
    FILE *snapshot_file = fopen("Snapshot.txt", "w");
    if (snapshot_file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    traverse_directory(dirname, snapshot_file);

    fclose(snapshot_file);
    printf("Snapshot created successfully.\n");
    return EXIT_SUCCESS;
}