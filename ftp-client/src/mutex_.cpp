#include <cstdio>
#include <mutex>
#include <condition_variable>
#include "mutex_.h"


static std::mutex m_print;

static std::mutex m_go_on;
static std::unique_lock<std::mutex>* lk_go_on;
static std::unique_lock<std::mutex>* lk_n_go_on;

static std::condition_variable c;



void pthread_mutex_lock_print_mutex()
{
	m_print.lock();
}
void pthread_mutex_unlock_print_mutex()
{

	m_print.unlock();
}



void pthread_mutex_lock_go_on_mutex()
{
	lk_go_on = new std::unique_lock<std::mutex>(m_go_on);
}
void pthread_mutex_unlock_go_on_mutex()
{
	delete lk_go_on;
}

void pthread_mutex_n_lock_go_on_mutex()
{
	lk_n_go_on = new std::unique_lock<std::mutex>(m_go_on);
}
void pthread_mutex_n_unlock_go_on_mutex()
{
	delete lk_n_go_on;
}

void pthread_cond_signal_cond()
{
	c.notify_one();
}

void pthread_cond_wait_cond()
{
	c.wait(*lk_go_on);
}


