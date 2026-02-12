/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:46 by loda-sil          #+#    #+#             */
/*   Updated: 2026/01/20 12:27:47 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int init_forks(t_data *data)
{
	int i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
		return (0);
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_mutexes(t_data *data)
{
	if (!init_forks(data))
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		destroy_forks(data);
		return (0);
	}
	if (pthread_mutex_init(&data->death_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		destroy_forks(data);
		return (0);
	}
	if (pthread_mutex_init(&data->meal_check_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->death_mutex);
		pthread_mutex_destroy(&data->print_mutex);
		destroy_forks(data);
		return (0);
	}
	return (1);
}

static void config_philo(t_data *data, int i)
{
	data->philos[i].id = i + 1;
	data->philos[i].meals_eaten = 0;
	data->philos[i].last_meal_time = 0;
	data->philos[i].data = data;
}

int init_philos(t_data *data)
{
	int i;

	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
		return (0);
	i = 0;
	while (i < data->num_philos)
	{
		config_philo(data, i);
		if (pthread_mutex_init(&data->philos[i].meal_mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&data->philos[i].meal_mutex);
			free(data->philos);
			return (0);
		}
		i++;
	}
	return (1);
}

int init_data(t_data *data)
{
	data->someone_died = 0;
	data->all_ate_flag = 0;
	if (!init_mutexes(data))
		return (0);
	if (!init_philos(data))
	{
		destroy_mutexes(data);
		return (0);
	}
	return (1);
}
