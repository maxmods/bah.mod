#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>

void timestamp(const char * fromFile, const char * toFile) {

	struct stat st;

	if (stat(fromFile,&st)) {
		printf("++++ stat failed for %s\n", fromFile);fflush(stdout);
		return;
	}

	struct timeval time[2];
	
	time[0].tv_sec   = st.st_atime;
	time[0].tv_usec  = 0;
	time[1].tv_sec   = st.st_mtime;
	time[1].tv_usec  = 0;

	if (utimes(toFile, &time)) {
		printf("++++ utimes failed for %s\n", toFile);fflush(stdout);

	}

}