#define	OPT_FLOOD	0x001
#define	OPT_INTERVAL	0x002
#define	OPT_NUMERIC	0x004
#define	OPT_QUIET	0x008
#define	OPT_RROUTE	0x010
#define	OPT_VERBOSE	0x020

extern unsigned options;
extern PING *ping;
extern int is_root;
extern u_char pattern[16];
extern int pattern_len;
extern unsigned long preload;
