# ifndef __FILESYSTEM_H__
# define __FILESYSTEM_H__



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int is_directory(const char*);
void list_directory(const char* path, char* buffer, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif


