#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>

#include <stdlib.h>

double getXrandrBrightness()
{
    FILE *fp;
    double value;
    char command_buffer[8];
    fp = popen("xrandr --verbose --current | grep -i Brightness | tail -c 5", "r");
    fgets(command_buffer, sizeof(command_buffer), fp);
    // printf("%s", command_buffer);
    pclose(fp);
    sscanf(command_buffer, "%lf", &value);
    // printf("%lf", value);

    return value;
}

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

int getWordLenght(char *wordArray[], int size_of_array)
{
    int item_of_array = size_of_array / sizeof(&wordArray);
    int wordLenght = 0;
    for (int i = 0; i < item_of_array; ++i)
    {
        wordLenght += strlen(wordArray[i]);
    }
    return ++wordLenght;
}

void getConcatedWord(char *command_buffer, char *wordArray[], int size_of_array, int wordLenght)
{
    int item_of_array = size_of_array / sizeof(&wordArray);
    char concatedWord[wordLenght];
    memset(concatedWord, '\0', wordLenght);
    for (int i = 0; i < item_of_array; ++i)
    {
        strcat(concatedWord, wordArray[i]);
    }
    // printf("%d\n", wordLenght);
    printf("%s\n", concatedWord);
    strcpy(command_buffer, concatedWord);
}

double main(int argc, char *argv[])
{
    int fd, wd;
    char *FILE_NAME1 = argv[1];
    char *FILE_NAME2 = argv[2];
    char Monitor[] = "eDP-1-1";

    struct inotify_event *event;
    // char buf[BUFSIZ];
    char buf[128];

    int target_value, max_value;
    double current_rate, target_rate;

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

    do
    {
        current_rate = getXrandrBrightness();
        target_value = read_value(FILE_NAME1);
        max_value = read_value(FILE_NAME2);
        target_rate = (double)target_value / max_value;
        // printf("%d\n%d\n%f\n", target_value, max_value, target_rate);
        // return fprintf(stdout, "%f", target_rate);
        // return target_rate;

        char command_rate[16];
        sprintf(command_rate, "%f", target_rate); // Convert double to char
        char *command[] = {"xrandr --output ", Monitor, " --brightness ", command_rate};
        int command_len = getWordLenght(command, sizeof(command));
        char *command_buffer = malloc(command_len);
        getConcatedWord(command_buffer, command, sizeof(command), command_len);
        printf("%s\n", command_buffer);
        pclose(popen(command_buffer, "r"));
        free(command_buffer);

    } while (read(fd, buf, sizeof(buf) - 1) > 0);

    return 0;
}