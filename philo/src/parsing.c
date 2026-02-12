/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 18:36:24 by loda-sil          #+#    #+#             */
/*   Updated: 2026/02/11 18:36:56 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int is_valid_number(char *str)
{
	int i;

	if (!str || !str[0])
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int validate_all_args(int argc, char **argv)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (!is_valid_number(argv[i]))
			return (0);
		i++;
	}
	return (1);
}

static int parse_positive_int(char *str, int *dest)
{
	int temp;

	if (!ft_atoi_safe(str, &temp))
		return (0);
	if (temp <= 0)
		return (0);
	*dest = temp;
	return (1);
}

static int	parse_optional_meals(int argc, char **argv, t_data *data)
{
	int	temp;

	if (argc == 6)
	{
		if (!ft_atoi_safe(argv[5], &temp))
			return (0);
		if (temp < 0)
			return (0);
		data->must_eat_count = temp;
	}
	else
		data->must_eat_count = -1;
	return (1);
}

int parse_arguments(int argc, char **argv, t_data *data)
{
	if (!validate_all_args(argc, argv))
		return (0);
	if (!parse_positive_int(argv[1], &data->num_philos))
		return (0);
	if (!parse_positive_int(argv[2], &data->time_to_die))
		return (0);
	if (!parse_positive_int(argv[3], &data->time_to_eat))
		return (0);
	if (!parse_positive_int(argv[4], &data->time_to_sleep))
		return (0);
	if (!parse_optional_meals(argc, argv, data))
		return (0);
	return (1);
}
