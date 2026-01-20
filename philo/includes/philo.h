/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loda-sil <loda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:58:09 by loda-sil          #+#    #+#             */
/*   Updated: 2026/01/20 17:44:36 by loda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>    // printf
# include <stdlib.h>   // malloc, free
# include <string.h>   // memset
# include <unistd.h>   // write, usleep
# include <sys/time.h> // gettimeofday
# include <pthread.h>  // pthread_create, pthread_join, pthread_mutex_*

# define INT_MAX 2147483647

typedef struct s_data
{
	int	num_philos;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	count_eat;
}	t_data;

#endif
