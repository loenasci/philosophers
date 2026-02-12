/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:52 by loda-sil          #+#    #+#             */
/*   Updated: 2026/01/20 12:27:53 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int simulation_should_stop(t_data *data)
{
	int should_stop;

	pthread_mutex_lock(&data->death_mutex);
	should_stop = data->someone_died;
	pthread_mutex_unlock(&data->death_mutex);
	return (should_stop);
}

static int	check_philosopher_death(t_philo *philo)
{
	long current_time;
	long time_since_meal;

	pthread_mutex_lock(&philo->meal_mutex);
	current_time = get_time_ms();
	time_since_meal = current_time - philo->last_meal_time;
	pthread_mutex_unlock(&philo->meal_mutex);
	if (time_since_meal >= philo->data->time_to_die)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		philo->data->someone_died = 1;
		pthread_mutex_unlock(&philo->data->death_mutex);
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%ld %d died\n", get_time_ms() - philo->data->start_time,
			philo->id);
		pthread_mutex_unlock(&philo->data->print_mutex);
		return (1);
	}
	return (0);
}

int check_death(t_data *data)
{
	int i;

	i = 0;
	while (i < data->num_philos)
	{
		if (check_philosopher_death(&data->philos[i]))
			return (1);
		i++;
	}
	return (0);
}

int check_all_ate(t_data *data)
{
	int i;
	int all_done;

	if (data->must_eat_count == -1)
		return (0);
	all_done = 1;
	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_lock(&data->philos[i].meal_mutex);
		if (data->philos[i].meals_eaten < data->must_eat_count)
			all_done = 0;
		pthread_mutex_unlock(&data->philos[i].meal_mutex);
		i++;
	}
	if (all_done)
	{
		pthread_mutex_lock(&data->death_mutex);
		data->someone_died = 1;
		pthread_mutex_unlock(&data->death_mutex);
	}
	return (all_done);
}

void *monitor_routine(void *arg)
{
	t_data *data;

	data = (t_data *)arg;
	while (1)
	{
		if (check_death(data))
			break ;
		if (check_all_ate(data))
			break ;
		usleep(1000);
	}
	return (NULL);
}
