/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:27:48 by loda-sil          #+#    #+#             */
/*   Updated: 2026/02/11 18:37:02 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void print_usage(void)
{
	printf("Usage: ./philo number_of_philos ");
	printf("time_to_die time_to_eat time_to_sleep ");
	printf("[times_each_philo_must_eat]\n");
}

static int create_philos(t_data *data)
{
	int i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&data->philos[i].thread,
						   NULL, philo_routine, &data->philos[i]) != 0)
		{
			pthread_mutex_lock(&data->death_mutex);
			data->someone_died = 1;
			pthread_mutex_unlock(&data->death_mutex);
			return (0);
		}
		i++;
	}
	return (1);
}

static int create_threads(t_data *data, pthread_t *monitor)
{
	data->start_time = get_time_ms();
	if (!create_philos(data))
		return (0);
	if (pthread_create(monitor, NULL, monitor_routine, data) != 0)
	{
		pthread_mutex_lock(&data->death_mutex);
		data->someone_died = 1;
		pthread_mutex_unlock(&data->death_mutex);
		return (0);
	}
	return (1);
}

static void wait_threads(t_data *data, pthread_t monitor)
{
	int i;

	pthread_join(monitor, NULL);
	i = 0;
	while (i < data->num_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
}

int main(int argc, char **argv)
{
	t_data		data;
	pthread_t	monitor;

	if (argc < 5 || argc > 6)
	{
		print_usage();
		return (error_exit("Invalid number of arguments"));
	}
	if (!parse_arguments(argc, argv, &data))
		return (error_exit("Invalid arguments"));
	if (!init_data(&data))
		return (error_exit("Initialization failed"));
	if (!create_threads(&data, &monitor))
	{
		wait_threads(&data, monitor);
		cleanup(&data);
		return (error_exit("Thread creation failed"));
	}
	wait_threads(&data, monitor);
	cleanup(&data);
	return (0);
}
