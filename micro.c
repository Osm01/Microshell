
#include "micro.h"

void print_error(const char *str)
{
	int i = 0;
	while (str[i])
		write(2, &str[i++], 1);
}

void cd(const char *path)
{
	if (!path || (!strcmp(path, ";")) || (!strcmp(path, "|")))
		return (print_error("error: cd: bad arguments\n"));
	if (chdir(path) == -1)
	{
		print_error("error: cd: cannot change ");
		print_error(path);
		print_error("\n");
	}
}

char **get_cmd(char *const *argv, int *start)
{
	char **cmd;
	int i;
	int index;
	int count;

	if (!argv && !argv[*start])
		return (NULL);
	i = *start;
	while (argv[i])
	{
		if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			break;
		i++;
	}
	count = (i - *start);
	if (count == 0)
		return (NULL);
	cmd = (char **)malloc(sizeof(char *) * (count + 1));
	if (!cmd)
		return (NULL);
	index = 0;
	i = (*start);
	while (index < count)
		cmd[index++] = argv[i++];
	cmd[index] = 0;
	*start += index;
	return (cmd);
}

void exe(char **cmd, char *const *env, int it_s_pipe, int *read_fd)
{
	pid_t pid;
	int fd[2];

	if (pipe(fd) == -1)
		return (print_error("error: fatal\n"));
	pid = fork();
	if (pid == -1)
		return (print_error("error fatal\n"));
	if (pid == 0)
	{
		close(fd[0]);
		dup2(*read_fd, 0);
		if (it_s_pipe == 1)
			dup2(fd[1], 1);
		close(fd[1]);
		if (execve(cmd[0], cmd, env) == -1)
			return (print_error("EROOR execute command"), print_error(cmd[0]), print_error("\n"), exit(0));
	}
	else
	{
		close(fd[1]);
		int status;
		waitpid(-1, &status, 0);
		if (*read_fd != 0)
			close(*read_fd);
		if (it_s_pipe)
			*read_fd = dup(fd[0]);
		close(fd[0]);
	}
}

int main(int argc, char *const *argv, char *const *env)
{
	char **cmd;
	int read_fd;
	int i;

	if (argc > 1)
	{
		read_fd = 0;
		i = 1;
		while (i < argc)
		{
			cmd = NULL;
			if (!strcmp(argv[i], "cd"))
				cd(argv[++i]);
			else
			{
				cmd = get_cmd(argv, &i);
				if (argv[i] && !strcmp(argv[i], "|"))
					exe(cmd, env, 1, &read_fd);
				else
					exe(cmd, env, 0, &read_fd);
				free(cmd);
			}
			i++;
		}
	}
	return (0);
}