#ifndef ORIGINAL_SOURCE

/* Replacement for tzfile.h */
#define DAYSPERNYEAR 365
#define SECSPERDAY 86400

/* BSD-specific function */
void strmode(mode_t mode, char *bp);

/* Unclassified */
char *user_from_uid(uid_t uid, int ignored);
char *group_from_gid(gid_t gid, int ignored);

#endif /* not ORIGINAL_SOURCE */
