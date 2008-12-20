/* This is called the, getting-around-WIN32-issues-without-having-to-hack-the-original-code, fix */

extern "C" {

#ifdef WIN32

#include <cdk.h>



#define gid_t int
#define uid_t int


struct passwd {
	char *pw_name;		/* Username.  */
	char *pw_passwd;		/* Password.  */
	uid_t pw_uid;			/* User ID.  */
	gid_t pw_gid;			/* Group ID.  */
	char *pw_gecos;		/* Real name.  */
	char *pw_dir;			/* Home directory.  */
	char *pw_shell;		/* Shell program.  */
};


int lstat(const char *file, struct stat *buf) {
	return stat(file, buf);
}

struct group
  {
    char *gr_name;		/* Group name.	*/
    char *gr_passwd;		/* Password.	*/
    gid_t gr_gid;		/* Group ID.	*/
    char **gr_mem;		/* Member list.	*/
  };

struct group * getgrgid (gid_t gid) {
	return NULL;
}

static struct passwd pw;

struct passwd * getpwuid (uid_t uid)
{
	//pw.pw_name = getlogin();
	pw.pw_uid = uid;
	//pw.pw_gid = getgid();
	pw.pw_shell = getenv("COMSPEC");
	//pw.pw_gecos = getlogin();
	//pw.pw_passwd = passwd;
	return &pw;
}

#endif

}