#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>

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

double getXrandrBrightness()
{
    FILE *fp;
    double value;
    char command_buffer[8];
    fp = popen("xrandr --verbose --current | grep -i Brightness | tail -c 5", "r");
    fgets(command_buffer, sizeof(command_buffer), fp);
    // printf("%s", command_buffer);
    pclose(fp);
    sscanf(command_buffer, "%lf", &value); // Convert char to double
    // printf("%lf", value);

    return value;
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
    // printf("%s\n", concatedWord);
    strcpy(command_buffer, concatedWord);
}

void contorllXrandrBrightness(char *monitorName, double brightnessRate)
{
    char command_rate[16];
    sprintf(command_rate, "%f", brightnessRate); // Convert double to char
    char *command[] = {"xrandr --output ", monitorName, " --brightness ", command_rate};
    int command_len = getWordLenght(command, sizeof(command));
    char *command_buffer = malloc(command_len);
    getConcatedWord(command_buffer, command, sizeof(command), command_len);
    // printf("%s\n", command_buffer);
    pclose(popen(command_buffer, "r"));
    free(command_buffer);
}

int main(int argc, char *argv[])
{
    int fd, wd;
    char *FILE_NAME1 = "/sys/class/backlight/intel_backlight/brightness";     // File address for backlight brightness.
    char *FILE_NAME2 = "/sys/class/backlight/intel_backlight/max_brightness"; // File address for backlight max brightness vlaue.
    char *Monitor;
    if (argv[1] == NULL)
    {
        printf("Usage: monitor-brightness [monitor_name]\n\nIf you want to find your monitor_name, use:\n   xrandr | grep ' connected ' | awk '{ print$1 }'\n");
        return 0;
    }
    else
    {
        Monitor = argv[1];
    }

    struct inotify_event *event;
    // char buf[BUFSIZ];
    char buf[128];
    int target_value, max_value;
    double current_rate, target_rate, compareGAP;

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
        target_rate = (double)target_value / max_value * 0.9 + 0.1; // Set theshold, let brightness rate never less than 0.1.
        compareGAP = target_rate - current_rate;
        if (compareGAP >= 0.006 || compareGAP <= -0.004)
        {
            contorllXrandrBrightness(Monitor, target_rate);
        }

    } while (read(fd, buf, sizeof(buf) - 1) > 0);

    return 0;
}
