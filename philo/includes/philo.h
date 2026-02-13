/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:58:09 by loda-sil          #+#    #+#             */
/*   Updated: 2026/02/11 17:57:45 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

# define INT_MAX 2147483647

typedef struct s_philo
{
	int				id;
	pthread_t		thread;
	long			last_meal_time;
	int				meals_eaten;
	pthread_mutex_t	meal_mutex;
	struct s_data	*data;
}	t_philo;

typedef struct s_data
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat_count;
	long			start_time;
	int				someone_died;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	t_philo			*philos;
	int				all_ate_flag;
	pthread_mutex_t	meal_check_mutex;
}	t_data;

// init.c
int		init_data(t_data *data);
int		init_mutexes(t_data *data);
int		init_philos(t_data *data);

// parsing.c
int		parse_arguments(int argc, char **argv, t_data *data);

// routine.c
void	*philo_routine(void *arg);

// actions.c
void	philo_eat(t_philo *philo);
void	philo_sleep(t_philo *philo);
void	philo_think(t_philo *philo);
void	philo_take_forks(t_philo *philo);
void	philo_drop_forks(t_philo *philo);

// monitor.c
void	*monitor_routine(void *arg);
int		check_death(t_data *data);
int		check_all_ate(t_data *data);
int		simulation_should_stop(t_data *data);

// utils.c
int		is_valid_number(char *str);
int		ft_atoi_safe(char *str, int *result);
long	get_time_ms(void);
void	smart_sleep(long milliseconds);
void	print_status(t_philo *philo, char *status);
int		error_exit(char *msg);

// cleanup.c
void	cleanup(t_data *data);
void	destroy_mutexes(t_data *data);
void	destroy_forks(t_data *data);

#endif
