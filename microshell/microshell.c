#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int	ft_strlen(char *str)
{
	int i = 0;
	while(str[i])
		i++;
	return i;
}

void	ft_cd(char **av)
{
	if (av[1] && !av[2])
	{
		if (chdir(*av)==-1)
		{
			write(2, "error: cd: cannot change directory to ", 38);
			write(2, av[1],ft_strlen(av[1]));
			write(2,"\n",1);
		}
	}
	else
		write(2, "error: cd: bad arguments\n", 25);
}

int number_command(char **av, int *index)
{
	int j;
	int i = 0;
	int number_cmd = 1;
	while (av[i])
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] == '|')
			{
				number_cmd++;
				break;
			}
			else if (av[i][j] == ';')
			{
				(*index)++;
				return number_cmd;
			}
			j++;
		}
		(*index)++;
		i++;
	}
	return (number_cmd);
}

void last_arg(char **av)
{
	int i = 0;
	int j = 0;
	while (av[i] && j != -1)
	{
		j = 0;
		while(av[i][j])
		{
			if (av[i][j] == '|' || av[i][j] == ';')
			{
				av[i] = 0;
				return ;
			}
			j++;
		}
		i++;
	}
}

char **next_command(char **av, int count)
{
	int i = 0;
	int j = 0;
	int cmd = 0;
	while (av[i] && cmd<count)
	{
		j = 0;
		while(av[i][j])
		{
			if (av[i][j] == '|')
				cmd++;
			j++;
		}
		if (!(cmd < count))
		{
			if (av[i][0] == '|')
				return (av + i+1);
			return (av + i);
		}
		i++;
	}
	return av;
}

int execute(char **av, char **env)
{
	int *pid;
	int pip[2];
	int prev[2];
	int index = 0;
	int ncmd = 0;
	int i = 0;
	char **dav;
	ncmd = number_command(av, &index);
	if (!strncmp(*av, "cd", -1))
	{
		ft_cd(av);
		return index;
	}
	pid = malloc(sizeof(int)*ncmd);
	while (i < ncmd)
	{
			if (i < ncmd -1)
			{
				if (pipe(pip)==-1)
				{
					write(2,"error: fatal\n",13);
					free(pid);
					exit(1);
				}
			}

			pid[i] = fork();
			dav = next_command(av, i);
			if (dav[0][0] == ';')
				return (index);
			if (pid[i] == 0)
			{
				if (i > 0)
				{
					dup2(prev[0], 0);
					close(prev[0]);
					close(prev[1]);
				}
				if (i < ncmd - 1)
				{
					dup2(pip[1], 1);
					close(pip[0]);
					close(pip[1]);
				}
				last_arg(dav);
				write(2,"cmd = ", 7);
				write(2, *dav, ft_strlen(*dav));
				write(2, "\n", 1);
				if (-1  == execve(*dav, dav, env))
				{
					write(2,"error: fatal\n",13);
					exit(1);
				}
				write(2,"error: cannot execute ", 22);
				write(2, *dav, ft_strlen(*dav));
				write(2, "\n", 1);
				exit (0);
			}
			else
			{
				if (i > 0)
				{
					close(prev[0]);
					close(prev[1]);
				}
				if (i < ncmd - 1)
				{
					prev[0] = pip[0];
					prev[1] = pip[1];
				}
			}
			i++;
	}
	ncmd = 0;
	while (wait(0)!=-1);
	free(pid);
	return index;
}

void parse_command(char **av, char **env)
{
	int i = 0;
	int j = 0;
	while (av[i])
		i++;
	while(j < i)
	{
		j += execute(av + j, env);
	}
}
int main(int ac , char **av, char **env)
{
	if (ac < 2)
		return (1);
	parse_command(av+1, env);
	return 0;
}
