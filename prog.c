#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>

pid_t pid_curr = -1;

void clear(void *buf)
{
	assert(buf != NULL);
	free(buf);
}

void func(int num_s)
{
	if (pid_curr != -1) {
		assert(kill(pid_curr, SIGINT) != -1);
		pid_curr = -1;
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, &func);
	while (1) {
		printf("command >> ");
		char *str = NULL;
		size_t temp = 0;
		ssize_t length = getline(&str, &temp, stdin); 
		assert(length != -1);
		if (str[length - 1] == '\n')
			str[length - 1] = '\0';
		length -= 1;
		
		if (strcmp(str, "exit") == 0)
			_exit(EXIT_SUCCESS);  
		if (length < 1) {                
			clear(str);
			continue;
		}
		int flag = 1;

		if (str[length - 1] == '&') {
			str[length - 1] = '\0';
			flag = 0;
		}
		
		char **splitted = NULL;
		size_t i = 0;
		char *lex = strtok(str, " "); 

		while (lex != NULL) {
			splitted = realloc(splitted, (i + 2) * sizeof(char *));
			assert(splitted != NULL);
			splitted[i++] = lex;
			lex = strtok(NULL, " ");
		}
		splitted[i] = NULL;

		pid_t pid = fork();
		assert(pid != -1);
		pid_curr = pid;

		if (pid_curr == 0) {
			if (execvp(splitted[0], splitted) == -1)
				puts("execution error");
			_exit(EXIT_SUCCESS);
		} else {
			if (flag) {
				int *status;
				wait(status);
				pid_curr = -1;
			}
		}
		clear(splitted);
		clear(str);
	}
	return EXIT_SUCCESS;
}