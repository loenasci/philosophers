/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:41 by loda-sil          #+#    #+#             */
/*   Updated: 2026/01/20 12:27:42 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void philo_think(t_philo *philo)
{
	print_status(philo, "is thinking");
}

void philo_sleep(t_philo *philo)
{
	print_status(philo, "is sleeping");
	smart_sleep(philo->data->time_to_sleep);
}

void philo_drop_forks(t_philo *philo)
{
	int left_fork;
	int right_fork;

	left_fork = philo->id - 1;
	right_fork = philo->id % philo->data->num_philos;
	pthread_mutex_unlock(&philo->data->forks[left_fork]);
	pthread_mutex_unlock(&philo->data->forks[right_fork]);
}

void philo_take_forks(t_philo *philo)
{
	int left;
	int right;

	left = philo->id - 1;
	right = philo->id % philo->data->num_philos;
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(&philo->data->forks[left]);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->data->forks[right]);
		print_status(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(&philo->data->forks[right]);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->data->forks[left]);
		print_status(philo, "has taken a fork");
	}
}

void philo_eat(t_philo *philo)
{
	philo_take_forks(philo);
	print_status(philo, "is eating");
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = get_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_mutex);
	smart_sleep(philo->data->time_to_eat);
	philo_drop_forks(philo);
}
