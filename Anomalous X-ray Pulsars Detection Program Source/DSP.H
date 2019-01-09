/********************** #defines ********************************/
#define DATA_BLOCKS 2048
#define LOOPS 10
#define MAXBUF 600 
#define D2msec   240000.0
#define LEN_TICK 0.055


/********************* Function Declarations *******************/


DWORD WINAPI acq_data(void* dummy);
int  filt_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc);
int  mean_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc);
int socket_talk (void);


/******************** Update Header Stuff  *********************/

int batDecode (unsigned short bat[], unsigned long int *mjd,
                  double *mjds);
int fill_hdr_strt(struct tphdr* hdr);
int upd_hdr_strt(struct tphdr* hdr, int beam);
int turn_hms(double turn, char *hms);
int turn_dms(double turn, char *dms);
int turn_deg(double turn, char * deg);