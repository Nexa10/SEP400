#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;
char name[100];

void change(char* str){
    char hey [64] = "is not the goat";
    char temp[64] ;
    if(strlen(name) > 0){
        strncpy(temp, name, sizeof(temp));

        
        memset(name,'\0', sizeof(name));
    }
    
   if (strcmp(temp, "name") == 0){
            strncpy(name, "Ronaldo ", sizeof(name));
            strcat(name, hey);
   }
   else if((strcmp(temp, "sleep")) == 0){
       sleep(2);
        //printf("Slept");
        cout << "slept" << endl;
   }
else{
            cout << "None" << endl;
    }
    
}

int main()
{
    strncpy(name, "name", 100);
    change(name);
    cout << "New Name: " << name << endl;

    return 0;
}