#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<time.h>
#include<errno.h>


void print4file(char* path) {
	struct stat stat;
	char mode[100];
	struct passwd *pw, *gr;
	struct tm lt;
	char timestr[255];

	lstat(path, &stat);
	
	pw = getpwuid(stat.st_uid);
	gr = getpwuid(stat.st_gid);

	//strmode(stat.st_mode, mode);
	sprintf(mode, "%d", stat.st_mode);

	localtime_r(&stat.st_mtime, &lt);
	strftime(timestr, 255, "%c", &lt);

	printf("%s\t%ld\t%s\t%s\t%ld\t%s\t%s\n", mode, stat.st_nlink, pw->pw_name, gr->pw_name, stat.st_size, timestr, path);
}

void writedir(char* dirname, int is_rec) {
	struct dirent* s;
	struct stat stat;
	char path[PATH_MAX];
	DIR* dir = opendir(dirname);

	printf("\n%s:\n", dirname);

	if (dir == NULL) {
		perror("open dir err");
	}
	while((s = readdir(dir)) != NULL) {
		if (dirname[1] != '\0') sprintf(path, "%s/%s", dirname, s->d_name);
		else sprintf(path, "%s", s->d_name);
		if (s->d_name[0] != '.')
			print4file(path);
	}

	if (!is_rec) return;
	rewinddir(dir);

	while((s = readdir(dir)) != NULL) {
		sprintf(path, "%s/%s", dirname, s->d_name);
		lstat(path, &stat);
		if((stat.st_mode & S_IFDIR) && (s->d_name[0] != '.')) {
			writedir(path, is_rec);
		}
	}
	closedir(dir);
	return;
}

int main(int argc, char **argv) {
	int lopt = 0;
	int Ropt = 0;
	char c;
	char cwd[PATH_MAX];

	while ((c = getopt(argc, argv, "lR")) != -1) {
		if (c == 'l') lopt = 1;
		else if (c == 'R') Ropt = 1;
	}
	if ((getcwd(cwd, PATH_MAX)) == NULL) {
		perror("failed to get cwd");
		exit(errno);
	}
	writedir(cwd, lopt && Ropt);
	return 0;
}
