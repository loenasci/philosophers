/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:54 by loda-sil          #+#    #+#             */
/*   Updated: 2026/01/20 12:27:55 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void init_philo_time(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = philo->data->start_time;
	pthread_mutex_unlock(&philo->meal_mutex);
}

static int handle_one_philo(t_philo *philo)
{
	if (philo->data->num_philos == 1)
	{
		print_status(philo, "has taken a fork");
		smart_sleep(philo->data->time_to_die);
		return (1);
	}
	return (0);
}

static int	check_meal_limit(t_philo *philo)
{
	int	done;

	if (philo->data->must_eat_count == -1)
		return (0);
	pthread_mutex_lock(&philo->meal_mutex);
	done = (philo->meals_eaten >= philo->data->must_eat_count);
	pthread_mutex_unlock(&philo->meal_mutex);
	return (done);
}

static void	philo_loop(t_philo *philo)
{
	while (!simulation_should_stop(philo->data))
	{
		philo_eat(philo);
		if (check_meal_limit(philo))
			break ;
		philo_sleep(philo);
		philo_think(philo);
	}
}

void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	init_philo_time(philo);
	if (handle_one_philo(philo))
		return (NULL);
	if (philo->id % 2 == 0)
		usleep(1000);
	philo_loop(philo);
	return (NULL);
}
