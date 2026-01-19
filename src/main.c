#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while(1){
    printf("$ ");
    char input[1024];
    char command[20];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")]='\0';
    if(strcmp(input, "exit")==0){
      break;
    }
    char *token = strtok(input, " ");
    if(token!=NULL){
      strcpy(command, token);
    }
    if(strcmp(command, "echo")==0){
      char *printText = strtok(NULL, "\0"); 
      if(printText!=NULL){
        printf("%s\n", printText);
      } 
    } else{
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}
