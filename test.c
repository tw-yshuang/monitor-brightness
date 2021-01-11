#include <stdio.h>
#include <unistd.h>
#include <sys/inotify.h>

#include <stdlib.h>
int read_value(char *FILENAME)
{
    char c_value[8];
    int value;
    FILE *fp;
    if ((fp = fopen(FILENAME, "r")) == NULL)
    {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // reads text until newline is encountered
    fscanf(fp, "%[^\n]", c_value);
    fclose(fp);
    // printf("Data from the file:\n%s", c_value);
    sscanf(c_value, "%d", &value);
    // printf("%d", value);
    return value;
}

double main(int argc, char *argv[])
{
    int fd;
    int wd;
    char *FILE_NAME1 = argv[1];
    char *FILE_NAME2 = argv[2];
    int target_value, max_value;
    double target_rate;

    struct inotify_event *event;
    // char buf[BUFSIZ];
    char buf[128];

    fd = inotify_init();
    if (fd < 0)
    {
        perror("inotify_init");
    }

    wd = inotify_add_watch(fd, FILE_NAME1, IN_CLOSE_WRITE);
    if (wd < 0)
    {
        perror("inotify_add-watch");
    }

    while (read(fd, buf, sizeof(buf) - 1) > 0)
    {
        target_value = read_value(FILE_NAME1);
        max_value = read_value(FILE_NAME2);
        target_rate = (double)target_value / max_value;
        // printf("%d\n%d\n%f\n", target_value, max_value, target_rate);
        return fprintf(stdout, "%f", target_rate);
        // return target_rate;
    }

    return 0;
}