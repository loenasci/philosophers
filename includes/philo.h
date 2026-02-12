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
	int				id;				// ID do filósofo (1 a N)
	pthread_t		thread;			// Thread do filósofo
	long			last_meal_time;	// Timestamp da última refeição
	int				meals_eaten;	// Contador de refeições
	pthread_mutex_t	meal_mutex;		// Protege last_meal_time e meals_eaten
	struct s_data	*data;			// Ponteiro para dados globais
}	t_philo;

typedef struct s_data
{
	int				num_philos;			// Número de filósofos
	int				time_to_die;		// Tempo para morrer (ms)
	int				time_to_eat;		// Tempo para comer (ms)
	int				time_to_sleep;		// Tempo para dormir (ms)
	int				must_eat_count;		// Vezes que deve comer (-1 se não obrigatório)
	long			start_time;			// Timestamp do início
	int				someone_died;		// Flag de morte (0 ou 1)
	pthread_mutex_t	*forks;				// Array de mutexes (garfos)
	pthread_mutex_t	print_mutex;		// Para sincronizar prints
	pthread_mutex_t	death_mutex;		// Para acessar someone_died
	t_philo			*philos;			// Array de filósofos
	int				all_ate_flag;		// Flag indicando que todos comeram suficiente
	pthread_mutex_t	meal_check_mutex;	// Protege all_ate_flag
}	t_data;

// init.c
int		init_data(t_data *data, int argc, char **argv);
int		init_mutexes(t_data *data);
int		init_philosophers(t_data *data);

// routine.c
void	*philosopher_routine(void *arg);

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

// cleanup.c
void	cleanup(t_data *data);
void	destroy_mutexes(t_data *data);

#endif
