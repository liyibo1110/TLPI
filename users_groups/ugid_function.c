#include "ugid_function.h"
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <sys/types.h>

char * userNameFromId(uid_t uid){
    struct passwd *pwd = getpwuid(uid);
    if(pwd == NULL){
        return NULL;
    }else{
	return pwd->pw_name;
    }
}

char * groupNameFromId(gid_t gid){
    struct group *grp = getgrgid(gid);
    if(grp == NULL){
	return NULL;
    }else{
	return grp->gr_name;
    }
}

uid_t userIdFromName(const char *name){
    
    if(name == NULL || *name == '\0'){
        return -1;
    }
    char *endptr;
    uid_t uid = strtol(name, &endptr, 10);     
    if(*endptr == '\0'){
	return uid;
    }

    struct passwd *pwd = getpwnam(name);
    if(pwd == NULL){
	return -1;
    }else{
	return pwd->pw_uid;
    }
}

gid_t groupIdFromName(const char *name){
    
    if(name == NULL || *name == '\0'){
        return -1;
    }
    char *endptr;
    gid_t gid = strtol(name, &endptr, 10);     
    if(*endptr == '\0'){
	return gid;
    }

    struct group *grp = getgrnam(name);
    if(grp == NULL){
	return -1;
    }else{
	return grp->gr_gid;
    }

}
