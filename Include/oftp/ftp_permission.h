#ifndef _FTP_PERMISSION_
#define _FTP_PERMISSION_

#define _POSIX_
#include <limits.h>
#include "stdio.h"
//1 user list

#define FTP_R	(1)		//读
#define FTP_C	(2)		//创建
#define FTP_D	(4)		//删除
#define FTP_W	(8)		//修改
#define FTP_X	(16)	//切换目录
#ifndef PATH_MAX
#define PATH_MAX 512
#endif

typedef struct  
{
	int permission;					//权限
	char username[48];				
	char password[48];
	char dir[PATH_MAX +1];
	char root[PATH_MAX + 1];
}ftp_user_t;

typedef struct{
ftp_user_t **pList;
int			usercount;
int			usermax;
}ftp_user_manager;


#ifdef SERVER_LIB
__declspec(dllexport) 
#endif	
	void			init_user_manager(int num);
#ifdef SERVER_LIB
__declspec(dllexport) 
#endif	
	int			add_user(ftp_user_t *user);
#ifdef SERVER_LIB
__declspec(dllexport) 
#endif
	void  destroy_user_manager();

ftp_user_t*		get_user(const char *username , const char *password);





#endif