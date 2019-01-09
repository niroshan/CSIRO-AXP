#include "dsp.h"
#include "TapeHdr\tape_hdr.h"
#include "DeskSystemStdHeaders\msgdef.h"

#ifndef STRUCT_SET_VAR
#define STRUCT_SET_VAR


struct channel_setup {
		int		num_chan_per_beam;
		int		num_beams;
		int		bits_sample_out;
		bool	subt_mean;
		float	ch1_cntr_freq;
		float	ch1_bw;
	};

	struct sampling_periods {
		int dacq;
		int strg;
	};

	struct socket {
		char  host_name[200];
		int	  port_add;
		bool  is_it_add;
		
	};

	struct data_file_name_details {
		char path_to_sve[500];
		bool  valid_name;
		volatile FILE *fptr;
		volatile int fd;
	};

struct aq {
	volatile short * piBuffer;
	volatile short *piHalfBuffer;
	volatile short TmpBuf[39][DATA_BLOCKS / 2 + 1];
	volatile float ResBuf[39][DATA_BLOCKS];
	volatile float *TmpBuf2[39];
	volatile float   *FinBuf[39];
	volatile unsigned int   GoodBuf[39][DATA_BLOCKS];
	volatile float  MaxVal;
	volatile float MinVal;
};

struct process_control {
	volatile bool do_i_die;
	volatile int  buff;
	volatile bool begin;
	volatile bool cleanup;
	volatile bool print_ok;
};

struct graphics_s {
	volatile bool time;
	volatile bool fft;
	volatile int  start_ch;
	volatile int  end_ch;
};

struct header {
	volatile struct tphdr   FileHdr[13];
	volatile int			block_num;
	volatile struct deskTag desk;
};


struct settings_struct {

	struct channel_setup			chn_stp;
	struct sampling_periods			smp_prds;
	struct socket					sck;
	struct data_file_name_details	dat_file;
	struct aq						acq;
	struct process_control			proc_cont;
	struct graphics_s				graphics;
	HWND hWnd;
	PAINTSTRUCT *ps;
	volatile int   what_next; 
	struct	header					TapeHdr;

} typedef SET_VAR;

#endif

extern volatile SET_VAR sh_mem;
extern volatile int bufpos;