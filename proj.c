#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#define MAX_FILENAME_LENGTH 256 

typedef struct Snapshot {
    char filename[MAX_FILENAME_LENGTH];
    mode_t permissions;
    uid_t owner;
    gid_t group;
    int size;
    time_t last_modified;
} Snapshot;

void create_snapshot(Snapshot snap) {
    FILE *output = fopen("Snapshot.txt", "w");
    if (!output) {
        print("error opening file");
        exit(10);
    }

    char permission_string[11];

    snprintf(permission_string, sizeof(permission_string), "%c%c%c%c%c%c%c%c%c%c",
        (S_ISDIR(snap.permissions)) ? 'd' : '-',
        (snap.permissions & S_IRUSR) ? 'r' : '-',
        (snap.permissions & S_IWUSR) ? 'w' : '-',
        (snap.permissions & S_IXUSR) ? 'x' : '-',
        (snap.permissions & S_IRGRP) ? 'r' : '-',
        (snap.permissions & S_IWGRP) ? 'w' : '-',
        (snap.permissions & S_IXGRP) ? 'x' : '-',
        (snap.permissions & S_IROTH) ? 'r' : '-',
        (snap.permissions & S_IWOTH) ? 'w' : '-',
        (snap.permissions & S_IXOTH) ? 'x' : '-');

    struct passwd *pwd;
    struct passwd *grp;
    pwd = getpwuid(snap.owner);
    grp = getpwuid(snap.group);

    fprintf(output, "Filename: %s\n", snap.filename);
    fprintf(output, "Permissions: %s\n", permission_string);
    fprintf(output, "Owner: %s\n", (snap.owner != NULL) ? owner->pw_name : "Unknown");
    fprintf(output, "Group: %s\n", (grp != NULL) ? grp->pw_name : "Unknown"); 
    fprintf(output, "Size: %d bytes\n", snap.size);
    fprintf(output, "Last Modified: %ld\n", snap.last_modified);
    fprintf(output, "\n");
}

void parseDir(char path[]) {

    int contor = 0;
    DIR *new_dir = opendir(path);
    
    if(new_dir == NULL) { 
        perror("Could not open the directory\n");
        exit(1);
    }

    char next_path[4096];
    struct dirent *dir_data;

    while( (dir_data = readdir(new_dir)) ) { 

        if( (!strcmp(dir_data -> d_name, ".")) || (!strcmp(dir_data -> d_name, "..")) )
            continue;
        
        strcpy(next_path, path);
        strcat(next_path, "/");
        strcat(next_path, dir_data -> d_name); 

        struct stat st;
        if( lstat(next_path, &st) < 0 ) {
            perror("Could not access the current file (bad path)\n");
            exit(2);
        }

        Snapshot snap;
        strcpy(snap.filename, next_path);
        snap.permissions = st.st_mode;
        snap.owner = st.st_uid;
        snap.group = st.st_gid;
        snap.size = st.st_size;
        snap.last_modified = st.st_mtime;

        if(S_IFDIR == (st.st_mode & S_IFMT))
            parseDir(next_path); 
        else
            create_snapshot(snap);
    }

    closedir(new_dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "argument error\n", argv[0]);
        return 1;
    }

    parseDir(argv[1]);

    return 0;
}