#define PING_MAX_DATALEN (65535 - MAXIPLEN - MAXICMPLEN)

extern unsigned options;
extern PING *ping;
extern u_char *data_buffer;
extern size_t data_length;

extern int ping_run (PING * ping, int (*finish) ());
extern int ping_finish (void);
extern void print_icmp_header (struct sockaddr_in *from,
			       struct ip *ip, icmphdr_t * icmp, int len);
