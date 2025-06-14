# ifndef __MUTEX___H__
# define __MUTEX___H__



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void pthread_mutex_lock_print_mutex();
void pthread_mutex_unlock_print_mutex();

void pthread_mutex_lock_go_on_mutex();
void pthread_mutex_unlock_go_on_mutex();

void pthread_mutex_n_lock_go_on_mutex();
void pthread_mutex_n_unlock_go_on_mutex();

void pthread_cond_signal_cond();
void pthread_cond_wait_cond();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif


