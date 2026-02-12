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

#include <philo.h>

static void	print_usage(void)
{
	printf("Usage: ./philo number_of_philos ");
	printf("time_to_die time_to_eat time_to_sleep ");
	printf("[times_each_philo_must_eat]\n");
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc < 5 || argc > 6)
	{
		print_usage();
		return (error_exit("Invalid number of arguments!"));
	}
	if (!parse_arguments(argc, argv, &data))
		return (error_exit("Invalid arguments!"));

	return (0);
}
