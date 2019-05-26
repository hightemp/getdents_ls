// http://www.olark.com/spw/2011/08/you-can-list-a-directory-with-8-million-files-but-not-with-ls/
#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
   long           d_ino;
   off_t          d_off;
   unsigned short d_reclen;
   char           d_name[];
};

#define BUF_SIZE (1024 * 1024 * 5)

void listdir(const char * dirname)
{
    int fd, nread;
    struct linux_dirent *d;
    int bpos;
    char d_type;
    char * buf = (char *) malloc(BUF_SIZE);

    fd = open(dirname, O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");

    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0)
            break;

        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            d_type = *(buf + bpos + d->d_reclen - 1);
            bpos += d->d_reclen;
            if(d->d_ino && strcmp(d->d_name, ".") && strcmp(d->d_name, ".."))
            {
                printf("%-10s ", (d_type == DT_REG) ?  "file" :
                        (d_type == DT_DIR) ?  "directory" :
                        (d_type == DT_FIFO) ? "FIFO" :
                        (d_type == DT_SOCK) ? "socket" :
                        (d_type == DT_LNK) ?  "symlink" :
                        (d_type == DT_BLK) ?  "block dev" :
                        (d_type == DT_CHR) ?  "char dev" : "???");
                printf("%s/%s\n", dirname, d->d_name);
                if(d_type == DT_DIR)
                {
                    int dirname_len = strlen(dirname);
                    char * subdir = (char*) calloc(1, PATH_MAX + 1);
                    strcat(subdir, dirname);
                    strcat(subdir + dirname_len, "/");
                    strcat(subdir + dirname_len + 1, d->d_name);
                    listdir(subdir);
                    free(subdir);
                }
            }
        }
    }

    close(fd);
    free(buf);
}

int
main(int argc, char *argv[])
{
    listdir(argc > 1 ? argv[1] : ".");
    exit(EXIT_SUCCESS);
}
