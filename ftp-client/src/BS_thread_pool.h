# ifndef __BS_THREAD_POOL_H__
# define __BS_THREAD_POOL_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void BS_thread_pool_post(void (*pf)(void*), void* vp);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

