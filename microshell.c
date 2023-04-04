#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>   //chdir,gethostname,getcwd
#include <ctype.h>    //isspace
#include <string.h>   //strtok, strcmp, strlen
#include <sys/wait.h>

pid_t forkVar;
#define MAX_LENGTH  512
#define MAX_BUF 2048

void set_home_directory(){
    char *home_dir =  getenv("HOME");
    if (chdir(home_dir)==-1)
       printf("Failed to set home directory\n");
    else
       chdir(home_dir);
}

void switch_to_home_dir() {
    char *home_dir = getenv("HOME");
    if (chdir(home_dir)==-1) {
        printf("Could not switch to home directory\n");
    }else{
       chdir(home_dir);
    }
}

void change_directory(char *destination){
    char *home_dir = getenv("HOME");
    if(strcmp(destination,"~")==0){
      if (chdir(home_dir)==-1)
         perror("Failed to change directory\n");
      else
         chdir(home_dir);
    }else{     
      if (chdir(destination)==-1)
         printf("The directory '%s' doesn't exist\n",destination);
      else
         chdir(destination);
    }
}

bool if_spaces(char *str){
    while (*str != '\0'){
        if (!isspace(*str))
           return false;
        str++;
    }
    return true;
}

void generate_prompt(char *prompt) {
    char current_working_dir[MAX_LENGTH];
    char host_name[MAX_LENGTH];
    char *user_name = getenv("USER");

    if  (gethostname(host_name,MAX_LENGTH)==-1){
        printf("Error in getting the hostname\n");
    }
    gethostname(host_name, MAX_LENGTH);
    if (getcwd(current_working_dir, MAX_BUF) != NULL) {
        snprintf(prompt, MAX_BUF, "\033[0;32m%s@%s\033[0;34m[%s]\033[0m$ ",user_name, host_name, current_working_dir);
    }
}

void split_command(char *cmd, char *argv[]) {
    int index = 0;
    int i = 0;
    int j = 0;
    int len = strlen(cmd);
    while (i < len) {
        while (isspace(cmd[i]) && i < len) i++;
        j = i;
        while (!isspace(cmd[j]) && j < len) j++;
        if (i != j) {
            cmd[j] = '\0';
            argv[index++] = &cmd[i];
        }
        i = j + 1;
    }
    argv[index] = NULL;
}

int main(){
    char cmd[MAX_BUF];
    char *arg[MAX_BUF];
    char prompt[MAX_BUF];
    set_home_directory();
      while (1)
      {
       generate_prompt(prompt); 
       printf("%s",prompt);
       fgets(cmd, MAX_BUF, stdin);
       split_command(cmd,arg);
       if(strlen(cmd)==0){
        continue;
       }
       if(if_spaces(cmd)){
        continue;
       }
       bool isRealized = false;

       if (strcmp("help", arg[0])==0){
                printf("Microshell UAM by Kiryl Khrystsenka\n\n");
                printf("I apologize for the delay in the implementation of my microshell\n\n");
                printf("Available commands at this moment:\n");
                printf("cd, help, exit, exec\n");
            isRealized=true;

        }else if (!strcmp("exit", arg[0])){
          
          return 0;

        }else if (strcmp(arg[0], "cd")==0){

            if (arg[1]!=NULL){
            change_directory(arg[1]);
            isRealized = true;
            }
            else {
                switch_to_home_dir();
                isRealized = true;
            }
        }else if(!isRealized){
            
        forkVar=fork();

        if (forkVar == 0)
        {
            execvp(arg[0],arg);
            if (execvp(arg[0],arg)==0)
                {
                    isRealized=true;
                    }
            else{
                fprintf(stderr,"%s\n",strerror(errno));
            }
            if (!isRealized){
                break;
                }
        }
        else if (forkVar == -1)
        {
            printf("creation of child procces is failed\n");
        }
        else
        {
            waitpid(forkVar,NULL,0);
        }
    }
}
}