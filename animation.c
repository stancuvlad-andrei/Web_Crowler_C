#include "animation.h"
#include <stdio.h>
#include <unistd.h>

void animation() {
    char spiderStart[] =
        "                              (\n"
        "                              )\n"
        "                       /\\  .-'''-.  /\\\n"
        "                       //\\/  ,,,  \\//\\\n"
        "                      |/\\| ,;;;;;, |/\\|\n"
        "                      ///\\\\;-'''-;///\\\\\n"
        "                     //  \\/   .   \\/  \\\\\n"
        "                    (| ,-_| \\ | / |_-, |)\n"
        "                      //`__\\.-.-./__`\\\\\n"
        "                     // /.-(() ())-.\\ \\\\\n"
        "                    (\\ |)   '---'   (| /)\n"
        "                     ` (|           |) `\n"
        "                       \\)           (/";

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < i; j++) {
            printf("                              (\n");
            printf("                              )\n");
        }

        printf("%s", spiderStart);

        for (int j = 0; j < 20 - i; j++) {
            printf("\n");
        }

        printf("----------------------------------------------------------------------------------\n");

        usleep(400000);
        printf("“\e[1;1H\e[2J”");
    }

    char spiderFinal[] =
        "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        "                            .-'''-.  \n"
        "                        / \\|   .   \\/ \n"
        "                       // \\|   .   / \\\n"
        "                      / ,-_| \\ | /|_-,| \n"
        "                     |//`__\\.-.-./__`\\\\ \n"
        "                     // /.-(() ())-.\\ \\\\\n"
        "                    (\\ |)   '---'   (| /)\n"
        "                     ` (|           |) `\n"
        "----------------------------------------------------------------------------------\n";

    printf("%s\n", spiderFinal);
}