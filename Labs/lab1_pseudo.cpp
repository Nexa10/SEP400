#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
char name[100];

void change(char *str)
{
    char hey[64] = "is not the goat";
    char *temp;
    strncpy(temp, str, sizeof(temp));
    // if (strlen(name) > 0)
    // {

    // }

    if (strncmp(str, "name", 4) == 0)
    {
        memset(name, '\0', sizeof(name));
        strncpy(name, "Ronaldo ", sizeof(name));
        strcat(name, hey);
    }
    else if (strncmp(str, "sleep", 5) == 0)
    {
        sleep(2);
        printf("Slept");
        // cout << "slept" << endl;
    }
    else
    {
        cout << "None" << endl;
    }
}

int main()
{
    strncpy(name, "name", 100);
    cout << "old Name: " << name << endl;
    change(name);
    printf("New name: %s\n", name);

    return 0;
}