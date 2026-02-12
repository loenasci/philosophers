/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:57 by loda-sil          #+#    #+#             */
/*   Updated: 2026/02/11 18:37:13 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi_safe(char *str, int *result)
{
	int		i;
	long	num;

	i = 0;
	num = 0;
	while (str[i])
	{
		num = num * 10 + (str[i] - '0');
		if (num > INT_MAX)
			return (0);
		i++;
	}
	*result = (int)num;
	return (1);
}

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	smart_sleep(long milliseconds)
{
	long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < milliseconds)
		usleep(500); // dorme 0.5ms e verifica de novo
}

void	print_status(t_philo *philo, char *status)
{
	long	timestamp;
	int		is_dead;

	pthread_mutex_lock(&philo->data->death_mutex);
	is_dead = philo->data->someone_died;
	pthread_mutex_unlock(&philo->data->death_mutex);
	if (!is_dead)
	{
		pthread_mutex_lock(&philo->data->print_mutex);
		timestamp = get_time_ms() - philo->data->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, status);
		pthread_mutex_unlock(&philo->data->print_mutex);
	}
}

int	error_exit(char *msg)
{
	printf("Error: %s\n", msg);
	return (1);
}
