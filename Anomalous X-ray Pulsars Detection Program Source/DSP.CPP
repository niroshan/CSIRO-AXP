#include "stdafx.h"
#include "daq_include\nidaqex.h"

volatile unsigned char constbuf[13][DATA_BLOCKS * 6 * 4];	// buf for binary file
volatile int bufpos;										// buf pos for above buf

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LOOPS 10
#define MAXBUF 600 
#define D2msec   240000.0
#define TKS_PER_SEC 18

/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	DWORD WINAPI acq_data(void* dummy)												**
**																					**
**	This is the heart of the acquiring system... It sets up memeory, and the device **
**	and then goes into a polled loop, checking if data has been collected... If data**
**	has been collected then we processit, and go back to waiting. Alot of the code	**
**	used here is National Instruments.....											**
**	Any questions should perhaps best be answered by studying the 2 examples progs	**
**	mentioned below...																**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/
DWORD WINAPI acq_data(void* dummy)
{
    /*
     * Local Variable Declarations: 
     */

	/*	Extract from............		"daq_include\nidaqex.h"
	typedef char i8;
	typedef unsigned char u8;
	typedef short i16;
	typedef unsigned short u16;
	typedef long i32;
	typedef unsigned long u32;
	typedef float f32;
	typedef double f64;
	*/
	// This is all national instrument defs here.... I basically got his code from
	// SCANsingleBufMultiRate.C & DAQdoubleBuf.C
	// However some defs are dynamic, due to very user selections of chans to observe.....
	// these are ontained using the variables which are stored in shared memory
	// NB shared memory is possible for vars with "volatile" next to them.....
	i16 iStatus = 0;
    i16 iRetVal = 0;
    i16 iDevice = 1;
	float lTimeout = 0.0;
    i16 iNumChans = sh_mem.chn_stp.num_beams * sh_mem.chn_stp.num_chan_per_beam; // num chans to obs
	float ratio_sin_sout = (float) floor((double) sh_mem.smp_prds.dacq / sh_mem.smp_prds.strg);
	int data_per_block = (int) floor(iNumChans*ratio_sin_sout*DATA_BLOCKS); // quantity of data in a block acquired
    f64 dSampRate = iNumChans * sh_mem.smp_prds.dacq;	// sampling rate
    f64 dScanRate = 0;									// leave at zero.. otherwise can vary rate at which it
														// it scans channels.............
	i16 iSampTB = 0;
    u16 uSampInt = 0;
    i16 iScanTB = 0;
    u16 uScanInt = 0;
    u32 ulCount = data_per_block * 2;		
	
	// these arrays are used to specigy what channels we want to acquire, the order in which they are acquired
	// the gain, and of course the sampling rate defined above (but at ratios of it... we just set to 1)
    static i16 * piChanVect;				
    static i16 * piGainVect;
	static i16 * piScanRateDivVect;
    static i16 piScanSeqVect[64] = {0};
	static u32 * pulSampsPerChanVect;
    i16 iIgnoreWarning = 0;
	int i, j;// m=0;
	i16 iScansPerSeq = 0;
    i16 iSampsPerSeq = 0;
    i16 iDAQstopped = 0;
	i16 iDBmodeON = 1;
	i16 iYieldON = 1;
	i16 iHalfReady = 0;
	u32 ulPtsTfr = 0;
	int HBuf_loc = 0;
	int cur_pos = 0;
	int HBuf_loc_mean = 0;
	int cur_pos_mean = 0;
	HANDLE hMutex_cleanup;  // this is like a semaphore... but we don't end up using it in this version
							// however, there is a need as we push this system further up.... That is switch from
							// a polled loop real time system to a multitasking system..... as we now only have 2
							// threads running concurrently... things are okay.....
	bufpos = 0;

	// init shared mem.............
	for(i=0; i < 39; i++)
	{
		if ((sh_mem.acq.TmpBuf2[i] = (float*) malloc(sizeof(float) * (size_t) DATA_BLOCKS )) == NULL)
			return 1;
		if((sh_mem.acq.FinBuf[i] = (float*)  malloc(sizeof(float) * (size_t) DATA_BLOCKS )) == NULL)
			return 1;
	}
	if ((sh_mem.acq.piBuffer		= (i16 * ) malloc(sizeof(i16) * (size_t) (data_per_block * 2))) == NULL)
		return 1;
	if (((sh_mem.acq.piHalfBuffer)  = (i16 * ) malloc(sizeof(i16) * (size_t) data_per_block * 2)) == NULL)
		return 1;

	// setup some global varaibles.....
	sh_mem.acq.MaxVal = (float) 1.0e-3;
	sh_mem.acq.MinVal = (float) 1.0e+3;
	sh_mem.proc_cont.do_i_die = FALSE;
	sh_mem.proc_cont.cleanup = FALSE;
//	sh_mem.proc_cont.print_ok = TRUE;

	// setup Mutex (Mutual Exculsion) but not used by any other funcs....
	hMutex_cleanup = CreateMutex(NULL, FALSE, "CLEAN_UP_MUTEX");
	WaitForSingleObject(hMutex_cleanup, INFINITE); // there should not be a delay here.....
//	sh_mem.proc_cont.cleanup = TRUE; //A lock.. so that we wait until this process is ready


	// grab resources.... memory
	if((piChanVect = (i16 * ) malloc(sizeof(i16) * (size_t) iNumChans)) == NULL)
		return 0;
	if ((piGainVect = (i16 * ) malloc(sizeof(i16) * (size_t) iNumChans)) == NULL)
		return 0;
	if ((piScanRateDivVect = (i16 * ) malloc(sizeof(i16) * (size_t) iNumChans)) == NULL)
		return 0;
	if ((pulSampsPerChanVect = (u32 * ) malloc(sizeof(u32) * (size_t) iNumChans)) == NULL)
		return 0;

	// setup time out for nat inst device.... that is the approx time we think it will take to
	//acquire data plus a little offset..... cur approx 1 sec offset.... works fine.....
	// This sets a timeout limit (#Sec * 18ticks/Sec) so that if there
    //is something wrong, the program won't hang on the SCAN_Op call.
	lTimeout = (float) DATA_BLOCKS * (float) ratio_sin_sout;
	lTimeout = (float) ( lTimeout/ (float) sh_mem.smp_prds.dacq)/ (float) LEN_TICK +  (float) TKS_PER_SEC + (float) 18;

	
	// init vectors for acquiring
	// while we initalize all 39 chans here.... only those defined by NumChans are actually used...
	for (i=0; i<iNumChans; i++)
	{
		piChanVect[i] = i;
		piGainVect[i] = 1;
		piScanRateDivVect[i] = 1;

		for(j=0; j < 1023; j++)
			sh_mem.acq.TmpBuf[i][j] = 0;
		
		for(j=0; j < DATA_BLOCKS; j++)
		{
			sh_mem.acq.TmpBuf2[i][j] = 0;
			sh_mem.acq.FinBuf[i][j] = 0;
			sh_mem.acq.GoodBuf[i][j] = 0;
		}
	}

	//Now we begin setting up the device for acquisition.....
	// the settings are scann all chans spec, with double buffering on

    /* Setup for multiple rate scanning. */

    iStatus = SCAN_Sequence_Setup(iDevice, iNumChans, piChanVect,
     piGainVect, piScanRateDivVect, &iScansPerSeq, &iSampsPerSeq);

    iRetVal = NIDAQErrorHandler(iStatus, "SCAN_Sequence_Setup",
     iIgnoreWarning);

    /* Obtain the scan sequence vector sometime before de-multiplexing
     acquired data. */

    iStatus = SCAN_Sequence_Retrieve(iDevice, iSampsPerSeq,
     piScanSeqVect);

    iRetVal = NIDAQErrorHandler(iStatus, "SCAN_Sequence_Retrieve",
     iIgnoreWarning);

	// sampling
    iStatus = DAQ_Rate(dSampRate, 0, &iSampTB, &uSampInt);

    iRetVal = NIDAQErrorHandler(iStatus, "DAQ_Rate/SampRate",
     iIgnoreWarning);

	// scanning
    iStatus = DAQ_Rate(dScanRate, 0, &iScanTB, &uScanInt);

    iRetVal = NIDAQErrorHandler(iStatus, "DAQ_Rate/ScanRate",
     iIgnoreWarning);

	iStatus = DAQ_DB_Config(iDevice, iDBmodeON);

    iRetVal = NIDAQErrorHandler(iStatus, "DAQ_DB_Config",
     iIgnoreWarning);

    /* Acquire data from multiple channels using interval scanning
     (both 'dSampRate' and 'dScanRate' are non-zero. NOTE: 'ulCount' must
     be an integer multiple of 'iSampsPerSeq'. */

    iStatus = SCAN_Start(iDevice, (short *)sh_mem.acq.piBuffer, ulCount, iSampTB,
     uSampInt, iScanTB, uScanInt);

    iRetVal = NIDAQErrorHandler(iStatus, "SCAN_Start", iIgnoreWarning);

	// okay now we've finsihed setting up the device...... lets setup the tape hdr...
	socket_talk(); // this grabs data from deskcom (only once, and dies)
	for(i=0; i < sh_mem.chn_stp.num_beams; i++) // dill all the hdrs
		upd_hdr_strt((struct tphdr*) &(sh_mem.TapeHdr.FileHdr[i]), i+1);
	sh_mem.TapeHdr.block_num = 1; //init vars
	bufpos = 0;

	while ((iStatus == 0) && (sh_mem.proc_cont.do_i_die != TRUE)) // loop until we get a kill request
	{   
        iStatus = DAQ_DB_HalfReady(iDevice, &iHalfReady, &iDAQstopped); // check if new data available
		
        if ((iHalfReady == 1) && (iStatus == 0)) // yes it is available
		{
			
            iStatus = DAQ_DB_Transfer(iDevice, (short *) sh_mem.acq.piHalfBuffer,
             &ulPtsTfr, &iDAQstopped); // grab the data

            iRetVal = NIDAQErrorHandler(iStatus, "DAQ_DB_Transfer", 0);

            /* Demultiplex (re-order) acquired data. */

			iStatus = SCAN_Sequence_Demux(iNumChans, piChanVect, ulCount/2,
			 (short *) sh_mem.acq.piHalfBuffer, iSampsPerSeq, piScanSeqVect, pulSampsPerChanVect);
			
			iRetVal = NIDAQErrorHandler(iStatus, "SCAN_Sequence_Demux",
			 iIgnoreWarning);

			/*//CALL FILTER HERE !!!!!!!!!!!!!! */   
			cur_pos = filt_data(iNumChans, cur_pos, data_per_block, &HBuf_loc);

			// CALL Mean function here.... Also 32bit to 8bit conversion of data..........
			cur_pos_mean = mean_data(iNumChans, cur_pos_mean, data_per_block, &HBuf_loc_mean);
			
			MessageBeep(0xFFFFFFFF); // finished new data.. let user know..... "BEEP" :-) To pc speaker
        }
        else
		{	
            iRetVal = NIDAQErrorHandler(iStatus, "DAQ_DB_HalfReady", 0);
        }
        iRetVal = NIDAQYield(iYieldON); // well give back time to OS, so that we arn't active polling, 
										// well okay we are, but not all the time... we check, then take
										// a time out, then check again, then another time out...
										// and so on.....
    }

    /* CLEANUP - Don't check for errors on purpose. */

    iStatus = DAQ_Clear(iDevice);
	free(piChanVect);
    free(piGainVect);
	free(piScanRateDivVect);
	free(pulSampsPerChanVect);
	for(i=0; i < 39; i++)
	{
		free((float*) sh_mem.acq.TmpBuf2[i]);
		free((float*) sh_mem.acq.FinBuf[i] );
	}
	free((i16 *) sh_mem.acq.piBuffer);
	free((i16 *)sh_mem.acq.piHalfBuffer);
	ReleaseMutex(hMutex_cleanup);
	return 0;
}


/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int  filt_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc)		**
**																					**
**	This is here filtering is done.... This algorithm reqs elements from the prev	**
**  and from the next buffer to complete..... Which then causes data to be delayed	**
**  by the time required to get the next batch of data b4 it can be completly		**
**	processed. There is nothing fancy here....										**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/

int  filt_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc)
{
	// Filter coefs.... obtained with the aid of Dick Ferris, and FirFilt program.........
	float filt_10[] = // 10 Hz Cutoff
	{
		(float) 0.010009, (float) 0.010007, (float) 0.010002, (float) 0.009993, (float) 0.009982, (float) 0.009966, (float) 0.009948, (float) 0.009926, (float) 0.009901, (float) 0.009872, (float) 0.009840, (float) 0.009805, (float) 0.009766, (float) 0.009725, (float) 0.009680, (float) 0.009632, (float) 0.009580, (float) 0.009526, (float) 0.009469, (float) 0.009408, (float) 0.009345, (float) 0.009278, (float) 0.009209, (float) 0.009136, (float) 0.009061, (float) 0.008983, (float) 0.008902, (float) 0.008819, (float) 0.008733, (float) 0.008644, (float) 0.008553, (float) 0.008460, (float) 0.008363, (float) 0.008265, (float) 0.008164, (float) 0.008061, (float) 0.007956, (float) 0.007849, (float) 0.007739, (float) 0.007628, (float) 0.007515, (float) 0.007400, (float) 0.007283, (float) 0.007164, (float) 0.007044, (float) 0.006922, (float) 0.006799, (float) 0.006674, (float) 0.006548, (float) 0.006421, (float) 0.006292, (float) 0.006163, (float) 0.006032, (float) 0.005901, 
		(float) 0.005768, (float) 0.005635, (float) 0.005501, (float) 0.005366, (float) 0.005231, (float) 0.005095, (float) 0.004959, (float) 0.004822, (float) 0.004685, (float) 0.004549, (float) 0.004411, (float) 0.004274, (float) 0.004137, (float) 0.004000, (float) 0.003863, (float) 0.003727, (float) 0.003591, (float) 0.003455, (float) 0.003319, (float) 0.003185, (float) 0.003051, (float) 0.002917, (float) 0.002784, (float) 0.002653, (float) 0.002522, (float) 0.002392, (float) 0.002263, (float) 0.002135, (float) 0.002008, (float) 0.001883, (float) 0.001759, (float) 0.001636, (float) 0.001515, (float) 0.001395, (float) 0.001276, (float) 0.001159, (float) 0.001044, (float) 0.000931, (float) 0.000819, (float) 0.000709, (float) 0.000600, (float) 0.000494, (float) 0.000390, (float) 0.000287, (float) 0.000186, (float) 0.000088, (float) -0.000009, (float) -0.000103, (float) -0.000195, (float) -0.000285, (float) -0.000373, (float) -0.000459, (float) -0.000542, (float) -0.000623,
		(float) -0.000702, (float) -0.000779, (float) -0.000853, (float) -0.000925, (float) -0.000994,
		(float) -0.001061, (float) -0.001126, (float) -0.001188, (float) -0.001248, (float) -0.001306, (float) -0.001361, (float) -0.001414, (float) -0.001464, (float) -0.001512, (float) -0.001557, (float) -0.001600, (float) -0.001641, (float) -0.001679, (float) -0.001715, (float) -0.001748, (float) -0.001779, (float) -0.001808, (float) -0.001834, (float) -0.001858, (float) -0.001880, (float) -0.001900, (float) -0.001917, (float) -0.001932, (float) -0.001945, (float) -0.001955, (float) -0.001964, (float) -0.001970, (float) -0.001974, (float) -0.001976, (float) -0.001976, (float) -0.001974, (float) -0.001970, (float) -0.001965, (float) -0.001957, (float) -0.001947, (float) -0.001936, (float) -0.001923, (float) -0.001908, (float) -0.001891, (float) -0.001873, (float) -0.001853, (float) -0.001832, (float) -0.001809, (float) -0.001785, (float) -0.001759, (float) -0.001732, (float) -0.001703, (float) -0.001674, (float) -0.001643, (float) -0.001610, (float) -0.001577, (float) -0.001543, 
		(float) -0.001507, (float) -0.001471, (float) -0.001433, (float) -0.001395, (float) -0.001356, (float) -0.001316, (float) -0.001275, (float) -0.001234, (float) -0.001192, (float) -0.001150, (float) -0.001106, (float) -0.001063, (float) -0.001019, (float) -0.000975, (float) -0.000930, (float) -0.000885, (float) -0.000839, (float) -0.000794, (float) -0.000748, (float) -0.000703, (float) -0.000657, (float) -0.000611, (float) -0.000565, (float) -0.000519, (float) -0.000474, (float) -0.000429, (float) -0.000383, (float) -0.000338, (float) -0.000294, (float) -0.000249, (float) -0.000205, (float) -0.000162, (float) -0.000119, (float) -0.000076, (float) -0.000034, (float) 0.000008, (float) 0.000049, (float) 0.000089, (float) 0.000129, (float) 0.000168, (float) 0.000206, (float) 0.000244, (float) 0.000281, (float) 0.000317, (float) 0.000352, (float) 0.000386, (float) 0.000420, (float) 0.000452, (float) 0.000484, (float) 0.000515, (float) 0.000545, (float) 0.000574, (float) 0.000602,
		(float) 0.000629, (float) 0.000655, (float) 0.000680, (float) 0.000704, (float) 0.000727, (float) 0.000749, (float) 0.000770, (float) 0.000789, (float) 0.000808, (float) 0.000826, (float) 0.000842, (float) 0.000858, (float) 0.000872, (float) 0.000885, (float) 0.000898, (float) 0.000909, (float) 0.000919, (float) 0.000928, (float) 0.000936, (float) 0.000943, (float) 0.000949, (float) 0.000953, (float) 0.000957, (float) 0.000960, (float) 0.000962, (float) 0.000962, (float) 0.000962, (float) 0.000961, (float) 0.000959, (float) 0.000955, (float) 0.000951, (float) 0.000946, (float) 0.000940, (float) 0.000934, (float) 0.000926, (float) 0.000918, (float) 0.000908, (float) 0.000898, (float) 0.000887, (float) 0.000875, (float) 0.000863, (float) 0.000850, (float) 0.000836, (float) 0.000821, (float) 0.000806, (float) 0.000790, (float) 0.000774, (float) 0.000757, (float) 0.000739, (float) 0.000721, (float) 0.000703, (float) 0.000684, (float) 0.000664, (float) 0.000644, (float) 0.000624, 
		(float) 0.000603, (float) 0.000582, (float) 0.000561, (float) 0.000539, (float) 0.000517, (float) 0.000495, (float) 0.000472, (float) 0.000450, (float) 0.000427, (float) 0.000404, (float) 0.000381, (float) 0.000358, (float) 0.000335, (float) 0.000311, (float) 0.000288, (float) 0.000265, (float) 0.000241, (float) 0.000218, (float) 0.000195, (float) 0.000172, (float) 0.000149, (float) 0.000126, (float) 0.000104, (float) 0.000081, (float) 0.000059, (float) 0.000037, (float) 0.000015, (float) -0.000006, (float) -0.000027, (float) -0.000048, (float) -0.000069, (float) -0.000089, (float) -0.000109, (float) -0.000129, (float) -0.000148, (float) -0.000166, (float) -0.000185, (float) -0.000203, (float) -0.000220, (float) -0.000237, (float) -0.000253, (float) -0.000270, (float) -0.000285, (float) -0.000300, (float) -0.000315, (float) -0.000329, (float) -0.000342, (float) -0.000355, (float) -0.000368, (float) -0.000380, (float) -0.000391, (float) -0.000402, (float) -0.000412, (float) -0.000422, 
		(float) -0.000431, (float) -0.000439, (float) -0.000447, (float) -0.000455, (float) -0.000462, (float) -0.000468, (float) -0.000474, (float) -0.000479, (float) -0.000483, (float) -0.000487, (float) -0.000491, (float) -0.000494, (float) -0.000496, (float) -0.000498, (float) -0.000499, (float) -0.000500, (float) -0.000500, (float) -0.000500, (float) -0.000499, (float) -0.000498, (float) -0.000496, (float) -0.000494, (float) -0.000491, (float) -0.000487, (float) -0.000484, (float) -0.000479, (float) -0.000475, (float) -0.000470, (float) -0.000464, (float) -0.000458, (float) -0.000452, (float) -0.000445, (float) -0.000438, (float) -0.000431, (float) -0.000423, (float) -0.000415, (float) -0.000406, (float) -0.000398, (float) -0.000388, (float) -0.000379, (float) -0.000369, (float) -0.000359, (float) -0.000349, (float) -0.000339, (float) -0.000328, (float) -0.000318, (float) -0.000307, (float) -0.000296, (float) -0.000284, (float) -0.000273, (float) -0.000261, (float) -0.000249, 
		(float) -0.000238, (float) -0.000226, (float) -0.000214, (float) -0.000202, (float) -0.000189, (float) -0.000178, (float) -0.000165, (float) -0.000153, (float) -0.000141, (float) -0.000129, (float) -0.000117, (float) -0.000105, (float) -0.000093, (float) -0.000081, (float) -0.000069, (float) -0.000057, (float) -0.000045, (float) -0.000034, (float) -0.000022, (float) -0.000011, (float) 0.000000, (float) 0.000011, (float) 0.000022, (float) 0.000033, (float) 0.000044, (float) 0.000054, (float) 0.000064, (float) 0.000074, (float) 0.000084, (float) 0.000093, (float) 0.000103, (float) 0.000112, (float) 0.000121, (float) 0.000130, (float) 0.000138, (float) 0.000145, (float) 0.000155, (float) 0.000162, (float) 0.000169, (float) 0.000176, (float) 0.000183, (float) 0.000190, (float) 0.000196, (float) 0.000202, (float) 0.000208, (float) 0.000214, (float) 0.000219, (float) 0.000224, (float) 0.000229, (float) 0.000233, (float) 0.000237, (float) 0.000241, (float) 0.000245, (float) 0.000248, 
		(float) 0.000251, (float) 0.000253, (float) 0.000256, (float) 0.000258, (float) 0.000260, (float) 0.000261, (float) 0.000263, (float) 0.000264, (float) 0.000266, (float) 0.000266, (float) 0.000267,
		(float) 0.000267, (float) 0.000267, (float) 0.000264, (float) 0.000266, (float) 0.000265, (float) 0.000264, (float) 0.000263, (float) 0.000261, (float) 0.000260, (float) 0.000258, (float) 0.000256, (float) 0.000254, (float) 0.000252, (float) 0.000249, (float) 0.000247, (float) 0.000244, (float) 0.000241, (float) 0.000238, (float) 0.000235, (float) 0.000232, (float) 0.000228, (float) 0.000225, (float) 0.000222, (float) 0.000218, (float) 0.000215, (float) 0.000212, (float) 0.000208, (float) 0.000205, (float) 0.000201, (float) 0.000198, (float) 0.000194, (float) 0.000192, (float) 0.000187, (float) 0.000185, (float) 0.000174, (float) 0.000169, (float) 0.000174, (float) 0.000166, (float) 0.000164, (float) 0.000159, (float) 0.000157, (float) 0.000152, (float) 0.000150, (float) 0.000146, (float) 0.000143, (float) 0.000140, (float) 0.000137, (float) 0.000134, (float) 0.000132, (float) 0.000129, (float) 0.000127, (float) 0.000124, (float) 0.000123, (float) 0.000120, (float) 0.000119,
		(float) 0.000117, (float) 0.000116, (float) 0.000115, (float) 0.000114, (float) 0.000113, (float) 0.000113, (float) 0.000112, (float) 0.000113, (float) 0.000113, (float) 0.000113, (float) 0.000114, (float) 0.000115, (float) -0.004897
	};
	float filt_15[] = // 15 Hz Cutoff
	{		
		(float) 0.014998, (float) 0.014993, (float) 0.014976, (float) 0.014948, (float) 0.014908, (float) 0.014857, (float) 0.014795, (float) 0.014723, (float) 0.014639, (float) 0.014544, (float) 0.014439, (float) 0.014323, (float) 0.014197, (float) 0.014060, (float) 0.013914, (float) 0.013757, (float) 0.013591, (float) 0.013416, (float) 0.013232, (float) 0.013038, (float) 0.012837, (float) 0.012626, (float) 0.012408, (float) 0.012182, (float) 0.011948, (float) 0.011708, (float) 0.011460, (float) 0.011206, (float) 0.010946, (float) 0.010680, (float) 0.010409, (float) 0.010133, (float) 0.009852, (float) 0.009566, (float) 0.009277, (float) 0.008984, (float) 0.008688, (float) 0.008389, (float) 0.008087, (float) 0.007784, (float) 0.007479, (float) 0.007172, (float) 0.006865, (float) 0.006557, (float) 0.006249, (float) 0.005941, (float) 0.005634, (float) 0.005328, (float) 0.005023, (float) 0.004720, (float) 0.004419, (float) 0.004121, (float) 0.003825, (float) 0.003532, (float) 0.003243, (float) 0.002957,
		(float) 0.002676, (float) 0.002399, (float) 0.002127, (float) 0.001859, (float) 0.001597, (float) 0.001340, (float) 0.001090, (float) 0.000845, (float) 0.000606, (float) 0.000374, (float) 0.000148, (float) -0.000071, (float) -0.000282, (float) -0.000487, (float) -0.000684, (float) -0.000874, (float) -0.001056, (float) -0.001230, (float) -0.001397, (float) -0.001556, (float) -0.001706, (float) -0.001849, (float) -0.001983, (float) -0.002110, (float) -0.002228, (float) -0.002338, (float) -0.002440, (float) -0.002533, (float) -0.002619, (float) -0.002696, (float) -0.002765, (float) -0.002827, (float) -0.002881, (float) -0.002926, (float) -0.002964, (float) -0.002995, (float) -0.003018, (float) -0.003034, (float) -0.003042, (float) -0.003044, (float) -0.003038, (float) -0.003026, (float) -0.003008, (float) -0.002983, (float) -0.002952, (float) -0.002915, (float) -0.002873, (float) -0.002825, (float) -0.002771, (float) -0.002713, (float) -0.002650, (float) -0.002582, (float) -0.002510, 
		(float) -0.002434, (float) -0.002354, (float) -0.002271, (float) -0.002184, (float) -0.002094, (float) -0.002001, (float) -0.001906, (float) -0.001808, (float) -0.001708, (float) -0.001607, (float) -0.001504, (float) -0.001400, (float) -0.001294, (float) -0.001188, (float) -0.001081, (float) -0.000974, (float) -0.000867, (float) -0.000760, (float) -0.000654, (float) -0.000548, (float) -0.000443, (float) -0.000338, (float) -0.000236, (float) -0.000134, (float) -0.000034, (float) 0.000064, (float) 0.000160, (float) 0.000254, (float) 0.000345, (float) 0.000434, (float) 0.000521, (float) 0.000604, (float) 0.000685, (float) 0.000763, (float) 0.000837, (float) 0.000909, (float) 0.000977, (float) 0.001041, (float) 0.001102, (float) 0.001160, (float) 0.001214, (float) 0.001264, (float) 0.001310, (float) 0.001353, (float) 0.001392, (float) 0.001427, (float) 0.001458, (float) 0.001485, (float) 0.001509, (float) 0.001528, (float) 0.001544, (float) 0.001556, (float) 0.001565, (float) 0.001570, (float) 0.001571, (float) 0.001568, 
		(float) 0.001562, (float) 0.001553, (float) 0.001540, (float) 0.001525, (float) 0.001505, (float) 0.001483, (float) 0.001458, (float) 0.001430, (float) 0.001399, (float) 0.001366, (float) 0.001330, (float) 0.001292, (float) 0.001251, (float) 0.001208, (float) 0.001164, (float) 0.001117, (float) 0.001069, (float) 0.001019, (float) 0.000967, (float) 0.000914, (float) 0.000860, (float) 0.000805, (float) 0.000749, (float) 0.000693, (float) 0.000635, (float) 0.000578, (float) 0.000519, (float) 0.000461, (float) 0.000402, (float) 0.000344, (float) 0.000286, (float) 0.000228, (float) 0.000170, (float) 0.000113, (float) 0.000057, (float) 0.000001, (float) -0.000053, (float) -0.000107, (float) -0.000160, (float) -0.000211, (float) -0.000261, (float) -0.000310, (float) -0.000357, (float) -0.000402, (float) -0.000446, (float) -0.000488, (float) -0.000529, (float) -0.000567, (float) -0.000604, (float) -0.000639, (float) -0.000671, (float) -0.000702, (float) -0.000731, (float) -0.000757, (float) -0.000781,
		(float) -0.000804, (float) -0.000824, (float) -0.000841, (float) -0.000857, (float) -0.000871, (float) -0.000882, (float) -0.000891, (float) -0.000898, (float) -0.000902, (float) -0.000905, (float) -0.000906, (float) -0.000904, (float) -0.000901, (float) -0.000895, (float) -0.000888, (float) -0.000878, (float) -0.000867, (float) -0.000854, (float) -0.000839, (float) -0.000823, (float) -0.000805, (float) -0.000785, (float) -0.000764, (float) -0.000742, (float) -0.000718, (float) -0.000693, (float) -0.000667, (float) -0.000640, (float) -0.000612, (float) -0.000582, (float) -0.000552, (float) -0.000522, (float) -0.000490, (float) -0.000458, (float) -0.000425, (float) -0.000392, (float) -0.000359, (float) -0.000325, (float) -0.000291, (float) -0.000257, (float) -0.000223, (float) -0.000189, (float) -0.000155, (float) -0.000122, (float) -0.000088, (float) -0.000055, (float) -0.000023, (float) 0.000010, (float) 0.000041, (float) 0.000072, (float) 0.000103, (float) 0.000132, (float) 0.000161, (float) 0.000189, (float) 0.000216, (float) 0.000242, (float) 0.000268, 
		(float) 0.000292, (float) 0.000315, (float) 0.000337, (float) 0.000358, (float) 0.000377, (float) 0.000396, (float) 0.000413, (float) 0.000429, (float) 0.000444, (float) 0.000458, (float) 0.000470, (float) 0.000481, (float) 0.000491, (float) 0.000499, (float) 0.000507, (float) 0.000513, (float) 0.000517, (float) 0.000521, (float) 0.000523, (float) 0.000524, (float) 0.000523, (float) 0.000522, (float) 0.000519, (float) 0.000515, (float) 0.000510, (float) 0.000505, (float) 0.000497, (float) 0.000489, (float) 0.000480, (float) 0.000470, (float) 0.000460, (float) 0.000448, (float) 0.000435, (float) 0.000422, (float) 0.000408, (float) 0.000393, (float) 0.000378, (float) 0.000362, (float) 0.000345, (float) 0.000328, (float) 0.000310, (float) 0.000292, (float) 0.000274, (float) 0.000255, (float) 0.000237, (float) 0.000218, (float) 0.000198, (float) 0.000179, (float) 0.000160, (float) 0.000140, (float) 0.000121, (float) 0.000101, (float) 0.000082, (float) 0.000063, (float) 0.000044, (float) 0.000025, (float) 0.000007,
		(float) -0.000011, (float) -0.000029, (float) -0.000046, (float) -0.000063, (float) -0.000080, (float) -0.000096, (float) -0.000111, (float) -0.000126, (float) -0.000141, (float) -0.000154, (float) -0.000168, (float) -0.000180, (float) -0.000192, (float) -0.000203, (float) -0.000214, (float) -0.000224, (float) -0.000233, (float) -0.000242, (float) -0.000249, (float) -0.000256, (float) -0.000263, (float) -0.000268, (float) -0.000273, (float) -0.000278, (float) -0.000281, (float) -0.000284, (float) -0.000286, (float) -0.000287, (float) -0.000288, (float) -0.000288, (float) -0.000287, (float) -0.000286, (float) -0.000284, (float) -0.000281, (float) -0.000278, (float) -0.000274, (float) -0.000270, (float) -0.000265, (float) -0.000259, (float) -0.000254, (float) -0.000247, (float) -0.000240, (float) -0.000233, (float) -0.000226, (float) -0.000217, (float) -0.000209, (float) -0.000200, (float) -0.000192, (float) -0.000182, (float) -0.000173, (float) -0.000163, (float) -0.000153, (float) -0.000143, (float) -0.000133,
		(float) -0.000123, (float) -0.000112, (float) -0.000102, (float) -0.000092, (float) -0.000081, (float) -0.000071, (float) -0.000060, (float) -0.000050, (float) -0.000040, (float) -0.000030, (float) -0.000020, (float) -0.000010, (float) -0.000001, (float) 0.000009, (float) 0.000018, (float) 0.000027, (float) 0.000036, (float) 0.000044, (float) 0.000052, (float) 0.000060, (float) 0.000067, (float) 0.000075, (float) 0.000082, (float) 0.000088, (float) 0.000094, (float) 0.000100, (float) 0.000106, (float) 0.000111, (float) 0.000115, (float) 0.000120, (float) 0.000124, (float) 0.000127, (float) 0.000131, (float) 0.000134, (float) 0.000136, (float) 0.000138, (float) 0.000140, (float) 0.000141, (float) 0.000142, (float) 0.000143, (float) 0.000143, (float) 0.000143, (float) 0.000142, (float) 0.000142, (float) 0.000141, (float) 0.000139, (float) 0.000138, (float) 0.000136, (float) 0.000133, (float) 0.000131, (float) 0.000128, (float) 0.000125, (float) 0.000122, (float) 0.000119, (float) 0.000115, (float) 0.000111, 
		(float) 0.000107, (float) 0.000103, (float) 0.000099, (float) 0.000095, (float) 0.000090, (float) 0.000085, (float) 0.000080, (float) 0.000076, (float) 0.000071, (float) 0.000066, (float) 0.000061, (float) 0.000056, (float) 0.000051, (float) 0.000046, (float) 0.000041, (float) 0.000036, (float) 0.000031, (float) 0.000026, (float) 0.000022, (float) 0.000017, (float) 0.000012, (float) 0.000007, (float) 0.000004, (float) -0.000001, (float) -0.000006, (float) -0.000009, (float) -0.000014, (float) -0.000017, (float) -0.000021, (float) -0.000024, (float) -0.000028, (float) -0.000031, (float) -0.000034, (float) -0.000037, (float) -0.000039, (float) -0.000042, (float) -0.000044, (float) -0.000046, (float) -0.000048, (float) -0.000050, (float) -0.000052, (float) -0.000053, (float) -0.000055, (float) -0.000056, (float) -0.000057, (float) -0.000058, (float) -0.000058, (float) -0.000059, (float) -0.000059, (float) -0.000059, (float) -0.000059, (float) -0.000059, (float) -0.000059, (float) -0.000058, (float) -0.000058,
		(float) -0.000058, (float) -0.000057, (float) -0.000057, (float) -0.000057, (float) -0.000059, (float) -0.000062, (float) -0.000070, (float) -0.000085, (float) -0.000115, (float) -0.000171, (float) -0.000277, (float) -0.000476, (float) 0.000379
	};
	float filt_20[] = // 20 Hz Cutoff
	{	
		(float) 0.020017, (float) 0.020004, (float) 0.019963, (float) 0.019895, (float) 0.019801, (float) 0.019680, (float) 0.019532, (float) 0.019359, (float) 0.019160, (float) 0.018937, (float) 0.018689, (float) 0.018417, (float) 0.018122, (float) 0.017805, (float) 0.017466, (float) 0.017106, (float) 0.016726, (float) 0.016328, (float) 0.015912, (float) 0.015478, (float) 0.015029, (float) 0.014565, (float) 0.014088, (float) 0.013598, (float) 0.013096, (float) 0.012585, (float) 0.012064, (float) 0.011536, (float) 0.011001, (float) 0.010461, (float) 0.009917, (float) 0.009371, (float) 0.008823, (float) 0.008274, (float) 0.007726, (float) 0.007181, (float) 0.006639, (float) 0.006101, (float) 0.005569, (float) 0.005044, (float) 0.004526, (float) 0.004017, (float) 0.003518, (float) 0.003030, (float) 0.002553, (float) 0.002089, (float) 0.001638, (float) 0.001201, (float) 0.000780, (float) 0.000374, (float) -0.000017, (float) -0.000390, (float) -0.000746, (float) -0.001084, (float) -0.001404, (float) -0.001705, 
		(float) -0.001988, (float) -0.002251, (float) -0.002496, (float) -0.002721, (float) -0.002927, (float) -0.003114, (float) -0.003281, (float) -0.003429, (float) -0.003558, (float) -0.003668, (float) -0.003760, (float) -0.003833, (float) -0.003888, (float) -0.003926, (float) -0.003947, (float) -0.003952, (float) -0.003940, (float) -0.003913, (float) -0.003872, (float) -0.003816, (float) -0.003746, (float) -0.003664, (float) -0.003569, (float) -0.003463, (float) -0.003347, (float) -0.003220, (float) -0.003085, (float) -0.002941, (float) -0.002790, (float) -0.002632, (float) -0.002468, (float) -0.002299, (float) -0.002126, (float) -0.001949, (float) -0.001769, (float) -0.001588, (float) -0.001405, (float) -0.001222, (float) -0.001039, (float) -0.000857, (float) -0.000677, (float) -0.000499, (float) -0.000324, (float) -0.000152, (float) 0.000015, (float) 0.000178, (float) 0.000335, (float) 0.000487, (float) 0.000633, (float) 0.000772, (float) 0.000904, (float) 0.001030, (float) 0.001147, (float) 0.001257, (float) 0.001359, 
		(float) 0.001453, (float) 0.001538, (float) 0.001615, (float) 0.001683, (float) 0.001743, (float) 0.001794, (float) 0.001837, (float) 0.001871, (float) 0.001896, (float) 0.001913, (float) 0.001922, (float) 0.001923, (float) 0.001916, (float) 0.001902, (float) 0.001880, (float) 0.001851, (float) 0.001815, (float) 0.001773, (float) 0.001725, (float) 0.001671, (float) 0.001612, (float) 0.001547, (float) 0.001478, (float) 0.001405, (float) 0.001328, (float) 0.001247, (float) 0.001164, (float) 0.001077, (float) 0.000989, (float) 0.000899, (float) 0.000808, (float) 0.000715, (float) 0.000622, (float) 0.000529, (float) 0.000436, (float) 0.000344, (float) 0.000253, (float) 0.000163, (float) 0.000074, (float) -0.000012, (float) -0.000096, (float) -0.000178, (float) -0.000257, (float) -0.000333, (float) -0.000405, (float) -0.000473, (float) -0.000538, (float) -0.000600, (float) -0.000656, (float) -0.000710, (float) -0.000758, (float) -0.000803, (float) -0.000842, (float) -0.000878, (float) -0.000909, (float) -0.000935, (float) -0.000957, 
		(float) -0.000974, (float) -0.000987, (float) -0.000995, (float) -0.000999, (float) -0.000999, (float) -0.000995, (float) -0.000987, (float) -0.000974, (float) -0.000959, (float) -0.000939, (float) -0.000916, (float) -0.000890, (float) -0.000861, (float) -0.000828, (float) -0.000794, (float) -0.000757, (float) -0.000719, (float) -0.000678, (float) -0.000636, (float) -0.000591, (float) -0.000545, (float) -0.000497, (float) -0.000453, (float) -0.000404, (float) -0.000355, (float) -0.000307, (float) -0.000258, (float) -0.000210, (float) -0.000162, (float) -0.000114, (float) -0.000068, (float) -0.000022, (float) 0.000022, (float) 0.000066, (float) 0.000108, (float) 0.000148, (float) 0.000187, (float) 0.000224, (float) 0.000259, (float) 0.000292, (float) 0.000323, (float) 0.000352, (float) 0.000379, (float) 0.000404, (float) 0.000425, (float) 0.000446, (float) 0.000463, (float) 0.000479, (float) 0.000492, (float) 0.000504, (float) 0.000513, (float) 0.000523, (float) 0.000528, (float) 0.000536, 
		(float) 0.000531, (float) 0.000526, (float) 0.000531, (float) 0.000524, (float) 0.000520, (float) 0.000512, (float) 0.000505, (float) 0.000494, (float) 0.000484, (float) 0.000472, (float) 0.000460, (float) 0.000446, (float) 0.000433, (float) 0.000419, (float) 0.000405, (float) 0.000389, (float) 0.000375, (float) 0.000360, (float) 0.000346, (float) 0.000332, (float) 0.000319, (float) 0.000306, (float) 0.000294, (float) 0.000283, (float) 0.000273, (float) 0.000264, (float) 0.000257, (float) 0.000251, (float) 0.000247, (float) 0.000244, (float) 0.000244, (float) 0.000244, (float) 0.000248, (float) -0.004868
	};
	float filt_25[] = // 25 Hz Cutoff
	{		
		(float) 0.025020, (float) 0.024993, (float) 0.024914, (float) 0.024783, (float) 0.024600, (float) 0.024366, (float) 0.024081, (float) 0.023748, (float) 0.023367, (float) 0.022939, (float) 0.022467, (float) 0.021952, (float) 0.021396, (float) 0.020802, (float) 0.020171, (float) 0.019506, (float) 0.018809, (float) 0.018083, (float) 0.017332, (float) 0.016556, (float) 0.015761, (float) 0.014947, (float) 0.014120, (float) 0.013280, (float) 0.012431, (float) 0.011577, (float) 0.010721, (float) 0.009864, (float) 0.009011, (float) 0.008164, (float) 0.007326, (float) 0.006499, (float) 0.005686, (float) 0.004891, (float) 0.004114, (float) 0.003360, (float) 0.002628, (float) 0.001923, (float) 0.001246, (float) 0.000598, (float) -0.000019, (float) -0.000604, (float) -0.001155, (float) -0.001670, (float) -0.002150, (float) -0.002594, (float) -0.003000, (float) -0.003369, (float) -0.003699, (float) -0.003992, (float) -0.004247, (float) -0.004465, (float) -0.004645, (float) -0.004789, (float) -0.004898, (float) -0.004971, (float) -0.005011, (float) -0.005018, 
		(float) -0.004993, (float) -0.004939, (float) -0.004855, (float) -0.004745, (float) -0.004610, (float) -0.004451, (float) -0.004270, (float) -0.004070, (float) -0.003851, (float) -0.003617, (float) -0.003368, (float) -0.003107, (float) -0.002836, (float) -0.002557, (float) -0.002272, (float) -0.001982, (float) -0.001690, (float) -0.001397, (float) -0.001105, (float) -0.000816, (float) -0.000532, (float) -0.000253, (float) 0.000017, (float) 0.000279, (float) 0.000531, (float) 0.000771, (float) 0.000999, (float) 0.001213, (float) 0.001413, (float) 0.001598, (float) 0.001767, (float) 0.001919, (float) 0.002055, (float) 0.002174, (float) 0.002276, (float) 0.002360, (float) 0.002427, (float) 0.002476, (float) 0.002509, (float) 0.002525, (float) 0.002524, (float) 0.002508, (float) 0.002476, (float) 0.002430, (float) 0.002370, (float) 0.002297, (float) 0.002212, (float) 0.002115, (float) 0.002008, (float) 0.001892, (float) 0.001767, (float) 0.001634, (float) 0.001495, 
		(float) 0.001351, (float) 0.001203, (float) 0.001051, (float) 0.000897, (float) 0.000742, (float) 0.000587, (float) 0.000432, (float) 0.000280, (float) 0.000130, (float) -0.000017, (float) -0.000159, (float) -0.000296, (float) -0.000427, (float) -0.000552, (float) -0.000670, (float) -0.000780, (float) -0.000882, (float) -0.000975, (float) -0.001060, (float) -0.001136, (float) -0.001202, (float) -0.001258, (float) -0.001306, (float) -0.001343, (float) -0.001371, (float) -0.001390, (float) -0.001399, (float) -0.001399, (float) -0.001390, (float) -0.001373, (float) -0.001348, (float) -0.001315, (float) -0.001274, (float) -0.001227, (float) -0.001173, (float) -0.001114, (float) -0.001049, (float) -0.000979, (float) -0.000906, (float) -0.000829, (float) -0.000749, (float) -0.000666, (float) -0.000582, (float) -0.000497, (float) -0.000411, (float) -0.000325, (float) -0.000239, (float) -0.000155, (float) -0.000072, (float) 0.000008, (float) 0.000086, (float) 0.000162, (float) 0.000233, (float) 0.000301, (float) 0.000365, (float) 0.000425, (float) 0.000480, 
		(float) 0.000530, (float) 0.000575, (float) 0.000616, (float) 0.000650, (float) 0.000680, (float) 0.000704, (float) 0.000723, (float) 0.000737, (float) 0.000745, (float) 0.000749, (float) 0.000747, (float) 0.000741, (float) 0.000730, (float) 0.000714, (float) 0.000695, (float) 0.000671, (float) 0.000644, (float) 0.000614, (float) 0.000580, (float) 0.000544, (float) 0.000506, (float) 0.000465, (float) 0.000423, (float) 0.000379, (float) 0.000334, (float) 0.000289, (float) 0.000243, (float) 0.000197, (float) 0.000151, (float) 0.000106, (float) 0.000061, (float) 0.000018, (float) -0.000024, (float) -0.000065, (float) -0.000104, (float) -0.000141, (float) -0.000176, (float) -0.000208, (float) -0.000239, (float) -0.000266, (float) -0.000292, (float) -0.000314, (float) -0.000334, (float) -0.000351, (float) -0.000365, (float) -0.000376, (float) -0.000385, (float) -0.000391, (float) -0.000395, (float) -0.000396, (float) -0.000394, (float) -0.000390, (float) -0.000385, (float) -0.000376, 
		(float) -0.000367, (float) -0.000354, (float) -0.000338, (float) -0.000326, (float) -0.000310, (float) -0.000293, (float) -0.000274, (float) -0.000255, (float) -0.000234, (float) -0.000214, (float) -0.000194, (float) -0.000173, (float) -0.000153, (float) -0.000133, (float) -0.000113, (float) -0.000094, (float) -0.000075, (float) -0.000057, (float) -0.000039, (float) -0.000023, (float) -0.000008, (float) 0.000007, (float) 0.000020, (float) 0.000032, (float) 0.000043, (float) 0.000052, (float) 0.000060, (float) 0.000067, (float) 0.000072, (float) 0.000076, (float) 0.000079, (float) 0.000080, (float) 0.000081, (float) 0.001911
	};
	float filt_50[] = // 50 Hz Cutoff
	{
		
		(float) 0.050042, (float) 0.049831, (float) 0.049202, (float) 0.048165, (float) 0.046735, (float) 0.044935, (float) 0.042793, (float) 0.040341, (float) 0.037617, (float) 0.034662, (float) 0.031520, (float) 0.028237, (float) 0.024860, (float) 0.021439, (float) 0.018019, (float) 0.014648, (float) 0.011369, (float) 0.008226, (float) 0.005254, (float) 0.002489, (float) -0.000041, (float) -0.002311, (float) -0.004302, (float) -0.006000, (float) -0.007398, (float) -0.008493, (float) -0.009289, (float) -0.009793, (float) -0.010018, (float) -0.009982, (float) -0.009706, (float) -0.009215, (float) -0.008536, (float) -0.007697, (float) -0.006730, (float) -0.005667, (float) -0.004538, (float) -0.003375, (float) -0.002206, (float) -0.001061, (float) 0.000037, (float) 0.001063, (float) 0.001998, (float) 0.002825, (float) 0.003532, (float) 0.004108, (float) 0.004547, (float) 0.004849, (float) 0.005012, (float) 0.005043, (float) 0.004947, (float) 0.004734, (float) 0.004418, (float) 0.004010, (float) 0.003527, (float) 0.002985, (float) 0.002401, (float) 0.001790, 
		(float) 0.001171, (float) 0.000557, (float) -0.000035, (float) -0.000592, (float) -0.001103, (float) -0.001557, (float) -0.001947, (float) -0.002266, (float) -0.002511, (float) -0.002679, (float) -0.002771, (float) -0.002789, (float) -0.002737, (float) -0.002620, (float) -0.002444, (float) -0.002218, (float) -0.001950, (float) -0.001649, (float) -0.001324, (float) -0.000986, (float) -0.000644, (float) -0.000306, (float) 0.000019, (float) 0.000325, (float) 0.000603, (float) 0.000849, (float) 0.001058, (float) 0.001228, (float) 0.001355, (float) 0.001441, (float) 0.001484, (float) 0.001488, (float) 0.001452, (float) 0.001383, (float) 0.001281, (float) 0.001155, (float) 0.001005, (float) 0.000841, (float) 0.000667, (float) 0.000481, (float) 0.000300, (float) 0.000122, (float) -0.000047, (float) -0.000206, (float) -0.000348, (float) -0.000474, (float) -0.000579, (float) -0.000663, (float) -0.000724, (float) -0.000764, (float) -0.000782, (float) -0.000781, (float) -0.000761, (float) -0.000725, (float) -0.000675, (float) -0.000614, (float) -0.000544,
		(float) -0.000468, (float) -0.000388, (float) -0.000308, (float) -0.000230, (float) -0.000155, (float) -0.000086, (float) -0.000025, (float) 0.000028, (float) 0.000070, (float) 0.000103, (float) 0.000124, (float) 0.000135, (float) 0.001995

	};

	float Filt[1023] = {0};
	float *filt_orig;
	int i, j, k, filt_pos = 0, filt_size = 0, filt_H_size = 0;
	int start;
	int cur_pos_bak = cur_pos;
	int dpb_sing_chan = data_per_block/NumChan;
	int samp_gap = sh_mem.smp_prds.dacq / sh_mem.smp_prds.strg;
	float run_tot = 0.0;
	int speed_up;
	
	/* Begin Filter Setup */
	// create the filter, as filter coefs are stored only to half the points (symentrical)
	if(sh_mem.smp_prds.strg == 10)
	{
		filt_orig = filt_10;
		filt_size = 1023;
		filt_H_size = 512;
	}
	else if(sh_mem.smp_prds.strg == 15)
	{
		filt_orig = filt_15;
		filt_size = 1023;
		filt_H_size = 512;
	}
	else if(sh_mem.smp_prds.strg == 20)
	{
		filt_orig = filt_20;
		filt_size = 511;
		filt_H_size = 256;
	}
	else if(sh_mem.smp_prds.strg == 25)
	{
		filt_orig = filt_25;
		filt_size = 511;
		filt_H_size = 256;
	}
	else if(sh_mem.smp_prds.strg == 50)
	{
		filt_orig = filt_50;
		filt_size = 255;
		filt_H_size = 128;
	}


	j = filt_H_size-1;
	for(i=0; i< filt_H_size ; i++)
	{
		Filt[i] = filt_orig[j];
		j--;
	}
	j = 0;
	for(i= filt_H_size-1; i< filt_size ; i++)
	{
		Filt[i] = filt_orig[j];
		j++;
	}

	/* End Filter Setup */

	if(cur_pos != 0) // only false for the first cycle, other wise completes the filtering
	{	
		
		for(i=0; i < NumChan; i++)
		{
			speed_up = i * dpb_sing_chan;
			start = *HBuf_loc;
			cur_pos = cur_pos_bak;

			while( start < dpb_sing_chan) // while there is still data
			{
				filt_pos = 0;
				run_tot = 0.0;
				for(j=(filt_size - (dpb_sing_chan - start)) + 1; j < filt_size; j++) // do from this batch of data
				{					
					run_tot = run_tot + ((float) sh_mem.acq.TmpBuf[i][j]) * Filt[filt_pos];
					filt_pos++;
				}
				for(k=0; filt_pos < filt_size; k++)	//do form the next batch of data
				{
					run_tot = run_tot + ((float) sh_mem.acq.piHalfBuffer[k + speed_up]) * Filt[filt_pos];
					filt_pos++;
				}		
				sh_mem.acq.ResBuf[i][cur_pos] = run_tot; // store the value
				cur_pos++;
				start = start + samp_gap;
			}
		}
	}

	for(i = 0; i < NumChan; i++) // do the start and middle part of data
	{
		start = 0;
		cur_pos = 0;
		speed_up = i * dpb_sing_chan;
		
		while((start + filt_H_size) < dpb_sing_chan) // while we do not get to the point where we need the next
		{											 // batch of data
			filt_pos = 0;
			run_tot = 0.0;
			if (start < filt_H_size) // start of filter, we need from prev cycle
			{
				for(j=start+1; j < filt_H_size; j++) 
				{
					run_tot = run_tot + ((float) (sh_mem.acq.TmpBuf[i][j + (filt_H_size - 2)]) * Filt[filt_pos]);
					filt_pos++;
				}
				for(k=0; filt_pos < filt_size; k++)
				{
					run_tot = run_tot + (((float) sh_mem.acq.piHalfBuffer[k + speed_up]) * Filt[filt_pos]);
					filt_pos++;
				}
			}
			else		// middle of filter
			{
				for(j=start - (filt_H_size-1); filt_pos < filt_size; j++)
				{
					run_tot = run_tot + ((float) sh_mem.acq.piHalfBuffer[j + speed_up]) * Filt[filt_pos];
					filt_pos++;
				}
			}
			sh_mem.acq.ResBuf[i][cur_pos] = run_tot;
			cur_pos++;
			start = start + samp_gap;
		}
	}

	for(i=0; i<NumChan; i++) // copy the end of the data stream, so that we can finish filtering in the next cycle
	{
		int speed_up = dpb_sing_chan - filt_size + i * dpb_sing_chan;
		for(j=0; j < filt_size; j++)
		{
			sh_mem.acq.TmpBuf[i][j] = sh_mem.acq.piHalfBuffer[speed_up + j ];
		}
	}
	*HBuf_loc= start; // update vals for next batch of data
	return cur_pos;
}




/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int  mean_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc)		**
**																					**
**	The concept is exactly the same as above.... only isntead of writing the		**
**  resulting value as the new data, we subtract it from the original data at that	**
**	point, hence removing the mean value... There are more efficient ways to do this**
**	but as we have to the buffer wrap arround, i choose to keep the alg identical to**
** the one above.., but the filter values are all the same.......					**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/

int  mean_data(int NumChan, int cur_pos, int data_per_block, int *HBuf_loc)
{
	int i, j, k, filt_pos = 0, filt_size = 127, filt_H_size = 64;
	int start;
	int cur_pos_bak = cur_pos;
	int dpb_sing_chan = DATA_BLOCKS;
	float run_tot = 0.0;
	//127 point mean.. thus filt coef = 1/127

	if(cur_pos != 0)
	{	
		
		for(i=0; i < NumChan; i++)
		{
			start = *HBuf_loc;
			cur_pos = cur_pos_bak;

			while( start < dpb_sing_chan)
			{
				filt_pos = 0;
				run_tot = 0.0;
				for(j=(filt_size - (dpb_sing_chan - start)) + 1; j < filt_size; j++)
				{					
					run_tot = run_tot + ((float) sh_mem.acq.TmpBuf2[i][j]) * (float) 0.0079;
					filt_pos++;
				}
				for(k=0; filt_pos < filt_size; k++)
				{
					run_tot = run_tot + ((float) sh_mem.acq.ResBuf[i][k]) * (float) 0.0079;
					filt_pos++;
				}		
				sh_mem.acq.FinBuf[i][cur_pos] = sh_mem.acq.ResBuf[i][cur_pos] - (float) run_tot;
				if( sh_mem.acq.MaxVal < sh_mem.acq.FinBuf[i][cur_pos])
					sh_mem.acq.MaxVal = sh_mem.acq.FinBuf[i][cur_pos];
				else if( sh_mem.acq.MinVal > sh_mem.acq.FinBuf[i][cur_pos])
					sh_mem.acq.MinVal = sh_mem.acq.FinBuf[i][cur_pos];
				cur_pos++;
				start++;
			}

		}
		// okay we have finsihed processing data.. so convert to 8bit data
		j=0;
		unsigned int masked;
		float ratio = 200 / 128.0; 
		float b4mask;
		for(k=0; k < NumChan; k++)
		{
			for(i=0; i < DATA_BLOCKS; i++)
			{			
				b4mask = b4mask = sh_mem.acq.FinBuf[k][i];
				masked = (int) 128.0 + (int) floor(b4mask / ratio);
				sh_mem.acq.GoodBuf[k][i] = masked; // we only want the least significant bits...... 8 bit data	
			}
		}
		for(k=0; k < sh_mem.chn_stp.num_beams; k++)
		{
			//dump data to file dump buffer........
			for(i=0; i < DATA_BLOCKS; i++)
			{
				constbuf[k][i*4     + bufpos*DATA_BLOCKS*4]	= (char) sh_mem.acq.GoodBuf[k*sh_mem.chn_stp.num_chan_per_beam][i];
				constbuf[k][i*4 + 1 + bufpos*DATA_BLOCKS*4]	= (char) sh_mem.acq.GoodBuf[k*sh_mem.chn_stp.num_chan_per_beam+1][i];
				constbuf[k][i*4 + 2 + bufpos*DATA_BLOCKS*4]	= (char) sh_mem.acq.GoodBuf[k*sh_mem.chn_stp.num_chan_per_beam+2][i];
				constbuf[k][i*4 + 3 + bufpos*DATA_BLOCKS*4]	= '0';
			}
			if(bufpos == 5) // every 6th cycle we dump the data to the file 49KB / beam
			{
				_write(sh_mem.dat_file.fd, (struct tphdr*) &(sh_mem.TapeHdr.FileHdr[k]), sizeof(struct tphdr));
				_commit(sh_mem.dat_file.fd); // flush file  buffer
				_write(sh_mem.dat_file.fd, (int *) constbuf[k], sizeof(unsigned char) * DATA_BLOCKS * 4 * 6);
				_commit(sh_mem.dat_file.fd);
			}
		}
		if(bufpos == 5) // setup vars for next blk
		{
			bufpos = 0;
			sh_mem.TapeHdr.block_num++;
			socket_talk();
			for(k=0; k < sh_mem.chn_stp.num_beams; k++)
				upd_hdr_strt((struct tphdr*) &(sh_mem.TapeHdr.FileHdr[k]), k+1);
		}
		else
			bufpos++;
	}

	for(i = 0; i < NumChan; i++)
	{
		start = 0;
		cur_pos = 0;
		
		while((start + filt_H_size) < dpb_sing_chan)
		{
			filt_pos = 0;
			run_tot = 0.0;
			if (start < filt_H_size)
			{
				for(j=start+1; j < filt_H_size; j++)
				{
					run_tot = run_tot + ((float) (sh_mem.acq.TmpBuf2[i][j])) * (float) 0.0079;// * Filt[filt_pos]);
					filt_pos++;
				}
				for(k=0; filt_pos < filt_size; k++)
				{
					run_tot = run_tot + (((float) sh_mem.acq.ResBuf[i][k]))* (float) 0.0079;// * Filt[filt_pos]);
					filt_pos++;
				}
			}
			else
			{
				for(j=start - (filt_H_size-1); filt_pos < filt_size; j++)
				{
					run_tot = run_tot + ((float) sh_mem.acq.ResBuf[i][j])* (float) 0.0079;// * Filt[filt_pos];
					filt_pos++;
				}
			}
			sh_mem.acq.FinBuf[i][cur_pos] = sh_mem.acq.ResBuf[i][cur_pos] - (float) run_tot;// / (float) filt_size;
			if( sh_mem.acq.MaxVal < sh_mem.acq.FinBuf[i][cur_pos])
				sh_mem.acq.MaxVal = sh_mem.acq.FinBuf[i][cur_pos];
			else if( sh_mem.acq.MinVal > sh_mem.acq.FinBuf[i][cur_pos])
				sh_mem.acq.MinVal = sh_mem.acq.FinBuf[i][cur_pos];
			cur_pos++;
			start++;
		}
	}

	for(i=0; i<NumChan; i++)
	{
		for(j=0; j < filt_size; j++)
		{
			sh_mem.acq.TmpBuf2[i][j] = sh_mem.acq.ResBuf[i][j];
		}
	}
	*HBuf_loc= start;
	return cur_pos;
}


////////////////////////////////////////////////SOCKET STUFF///////////////////////////////////////////////////////////
/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int socket_talk (void)															**
**																					**
** grabs the current deskCom status... Std socket.. using winsock					**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/

int socket_talk (void)
{
    int s;
    struct sockaddr_in a;
    struct hostent *h;
	struct deskTag deskCopy;
    WSADATA wsaData;
	

    if(WSAStartup(0x101, &wsaData))
    {
		MessageBox(sh_mem.hWnd, (LPCTSTR) "Unable to initialize WinSock library.", "ERROR", MB_OK);
        return 0;
    }

    h = gethostbyname((char *) "scutum.atnf.csiro.au");
    if (h == NULL)
    {
		MessageBox(sh_mem.hWnd, (LPCTSTR) "Cannot resolve hostname", "ERROR", MB_OK);
        WSACleanup();
        return 0;
    }


    a.sin_family = AF_INET;
    a.sin_port = htons(4005); // port address IF THIS CHANGES PLS FIX IT....
    memcpy(&(a.sin_addr.s_addr), h->h_addr, sizeof(int));
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s == 0)
    {	
		MessageBox(sh_mem.hWnd, (LPCTSTR) "Cannot Get Socket Connection", "ERROR", MB_OK);
		sh_mem.proc_cont.do_i_die = TRUE;
		closesocket(s);
        WSACleanup();
        return 0;
    }

    if (connect(s, (struct sockaddr *)&a, sizeof(a)))
    {
		MessageBox(sh_mem.hWnd, (LPCTSTR) "Cannot Get Socket Connection", "ERROR", MB_OK);
		closesocket(s);
        WSACleanup();
		sh_mem.proc_cont.do_i_die = TRUE;
		
        return 0;
    }

	if (send(s, "PC\n", 2, 0) != 2) // poll "PC" to get pc data.. (little, big endian blah, blah, blah
	{
		MessageBox(sh_mem.hWnd, (LPCTSTR) "Unable to POLL DESK.", "ERROR", MB_OK);
		closesocket(s);
        WSACleanup();
		sh_mem.proc_cont.do_i_die = TRUE;
	}
	else
	{
		int nb = recv(s, (char *)&deskCopy, sizeof(deskTag) / sizeof(char), 0); // grab socket message

		if (nb <= 0) // not valid data, string <= 0
		{
			MessageBox(sh_mem.hWnd, (LPCTSTR) "Unable to obtain desk status", "ERROR", MB_OK);
			closesocket(s);
			WSACleanup();
			sh_mem.proc_cont.do_i_die = TRUE;
		}
		else //copy into global memory
		{
			memcpy((struct deskTag*) &(sh_mem.TapeHdr.desk),(struct deskTag*) &(deskCopy), sizeof(struct deskTag));
		}
	}
    closesocket(s);
    WSACleanup();
	return 0;
}

/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int batDecode (unsigned short bat[], unsigned long int *mjd,  double *mjds)		**													**
**																					**
**  get mjd and mjds - writen by Mike Kersteven										**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/


int batDecode (unsigned short bat[], unsigned long int *mjd,
                  double *mjds)
{
        unsigned long b1, b2;
        double   f1, f2;
        double   fBat;
        double   fmjd;


        b1 = (unsigned short)bat[0] + (unsigned short)bat[1]*65536;
        b2 = (unsigned short)bat[2] + (unsigned short)bat[3]*65536;

        f1 = (double) (b1*1.0E-6);
        f2 = (double) (b2*4294.967296);  


        fBat =  (f1 + f2);
        fmjd  = fBat / 86400.0;

        *mjd  = (int) fmjd;
        *mjds = fBat - *mjd * 86400.0;


        return(0);
}        


/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int fill_hdr_strt(struct tphdr* hdr)											**	
**																					**
** Fill hdr for first time, with non changing data									**
** Std string manips, copies strings without null char, using memcpy				**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/

int fill_hdr_strt(struct tphdr* hdr)
{
	char tmp[100] = {0};
	char def = ' ';
	struct deskTag deskCopy;
	unsigned long int mjd;
    double mjds;

	memcpy((struct deskTag*) &(deskCopy), (struct deskTag*) &(sh_mem.TapeHdr.desk), sizeof(struct deskTag));

	memset(hdr, def, sizeof(struct tphdr));	// Set all the chars to ' '.....

	memcpy(hdr->program, "AXPRFW", 6);				//Program Name
	memcpy(hdr->version, "01.0", 4);				//Version "v.vv"

	// Get mjd & mjds
	batDecode((unsigned short *) deskCopy.co.cobat, &mjd, &mjds);

	// Check if date is acceptable....
	if((mjd <= -2395522.0) || (mjd >= 1.0e+9))
		MessageBox(sh_mem.hWnd, "MJD is corrupt...", "ERROR!!!", MB_OK);
	else
	{
		// Sperate day and fraction
		int f= mjd % 1;

		if (f < 0)
			f = f + 1;

		int d = mjd - f;

		int jd = d + 2400001;

		int n4 = 4 * (jd+((6*((4*jd-17918)/146097))/4 + 1) / 2 - 37);
		int nd10 = (10 * ((n4 - 237) % 1461) / 4) + 5;

		if ((n4/1461-4712 >= 1900) && (n4/1461-4712 < 2000))
			n4 = n4/1461-4712 - 1900;
		else if (n4/1461-4712 > 2000)
			n4 = n4/1461-4712 - 2000;

		sprintf(tmp, "%02d%02d%02d  ", n4, ((nd10/306 + 2) % 12) + 1, ((nd10 % 306) / 10));
	}
	memcpy(hdr->date, tmp, strlen(tmp));						// UT date "yymmdd  "
	
	sprintf(tmp, "%8d", mjd);
	memcpy(hdr->mjd, tmp, strlen(tmp));						// MJD at 00h UT

	sprintf(tmp, " %.2d:%.2d:%.2d", (int) floor(mjds/(60 * 60)), ((int) floor(mjds/60)) % 60, ((int) mjds) % 60);
	memcpy(hdr->ut_start, tmp, strlen(tmp));					// UT at file start  " hh:mm:ss.ssssss:

	turn_hms((float) (deskCopy.co.haapp +  deskCopy.co.radeca[0])/ (float) 86400000.0, tmp);
	memcpy(hdr->lst_start, tmp, strlen(tmp));

	memcpy(hdr->coord_type, " 4", 2);				// Galactic = " 4"


	float ra = (float) ((float) deskCopy.co.radec5[0] / (float)86400000.0);
	turn_hms(ra, tmp);
	memcpy(hdr->ra_start, tmp, strlen(tmp));					// ra_start = " hh:mm:ss.ssss"

	float dec = (float) ((float) deskCopy.co.radec5[1] / (float)86400000.0);
	turn_dms(dec, tmp);
	memcpy(hdr->dec_start, tmp, strlen(tmp));				// dec_start = "-dd:mm:ss.sss"

	float gl = (float) ((float) deskCopy.co.galact[0] / (float) 86400000.0);
	float gb = (float) ((float) deskCopy.co.galact[1] / (float) 86400000.0);

	turn_deg(gl, tmp);
	memcpy(hdr->gl_start, tmp, strlen(tmp));					// gl_start = "ddd.dddd"
	turn_deg(gb, tmp);
	memcpy(hdr->gb_start, tmp, strlen(tmp));					// gb_start = "-dd.dddd"

	memcpy(hdr->nfilter, " 1", 2);					// " 1" => one filter system....

	sprintf(tmp, "%7.5f", sh_mem.chn_stp.ch1_bw);
	memcpy(hdr->chanbw[0], tmp, 8);

	sprintf(tmp, "  %2d", sh_mem.chn_stp.num_chan_per_beam);
	memcpy(hdr->nchan[0], tmp, strlen(tmp));					// nchan => "  dd"
	memcpy(hdr->nchan[1], "  00", 4);				// second filter has no channels

	sprintf(tmp, "%12.5f", sh_mem.chn_stp.ch1_cntr_freq);
	memcpy(hdr->freq_ch1[0], tmp, 12);				// "ffffff.fffff"

	sprintf(tmp, "%12.9f", (float) 1000.0 / (float) sh_mem.smp_prds.strg);
	memcpy(hdr->samp_int[0], tmp, strlen(tmp));				// "mm.mmmmmmmmm"

	memcpy(hdr->samp_grp[0], "   1", strlen("   1"));				// always equals 1

	sprintf(tmp, "    %d", DATA_BLOCKS * 6);
	memcpy(hdr->samp_blk, tmp, strlen(tmp));						// "    dddd"

	sprintf(tmp, "%8.3f", ((float) 6 * (float) (sh_mem.smp_prds.dacq / sh_mem.smp_prds.strg) * (float) DATA_BLOCKS) / (float) 1000.0);
	memcpy(hdr->blk_sec, tmp, strlen(tmp));						// "ss.sssss"

	memcpy(hdr->data[0], "3", 1);						// "3" = normal

	sprintf(tmp, "%4d", sh_mem.chn_stp.num_beams);
	memcpy(hdr->nbeam, tmp, strlen(tmp));						// "  dd"

	sprintf(tmp, "%2d", sh_mem.chn_stp.bits_sample_out);
	memcpy(hdr->nbits[0], tmp, strlen(tmp));						// always " 8"
	
	memcpy(hdr->telid, "PARKES", strlen("PARKES"));					// onyl for use on parkes.....
	return 0;
}

/*************************************************************************************
**************************************************************************************
**																					**
**																					**
**	int upd_hdr_strt(struct tphdr* hdr, int beam)									**	
**																					**
** Fill hdr for first time, with changing data										**
** Std string manips, copies strings without null char, using memcpy				**
**																					**
**																					**
**************************************************************************************
*************************************************************************************/

int upd_hdr_strt(struct tphdr* hdr, int beam)
{
	char tmp[100] = {0};
	char def = ' ';
	struct deskTag deskCopy;
	unsigned long int mjd;
    double mjds;

	memcpy((struct deskTag*) &(deskCopy), (struct deskTag*) &(sh_mem.TapeHdr.desk), sizeof(struct deskTag));

	sprintf(tmp, "%8d", sh_mem.TapeHdr.block_num);
	memcpy(hdr->blk_cntr, tmp, strlen(tmp));					// block number.....

	
	// Get mjd & mjds
	batDecode((unsigned short *) deskCopy.co.cobat, &mjd, &mjds);

	sprintf(tmp, " %.2d:%.2d:%.2d", (int) floor(mjds/(60 * 60)), ((int) floor(mjds/60)) % 60, ((int) mjds) % 60);
	memcpy(hdr->ut_blk, tmp, strlen(tmp));					// UT at file blk  " hh:mm:ss.ss"

	turn_hms((float) (deskCopy.co.haapp +  deskCopy.co.radeca[0])/ (float) 86400000.0, tmp);
	memcpy(hdr->lst_blk, tmp, strlen(tmp));					// LST at file blk  " hh:mm:ss.ss"

	float ra = (float) ((float) deskCopy.co.radec5[0] / (float)86400000.0);
	turn_hms(ra, tmp);
	memcpy(hdr->ra_blk, tmp, strlen(tmp));					// ra_blk = " hh:mm:ss.ssss"

	float dec = (float) ((float) deskCopy.co.radec5[1] / (float)86400000.0);
	turn_dms(dec, tmp);
	memcpy(hdr->dec_blk, tmp, strlen(tmp));					// dec_blk = "-dd:mm:ss.sss"

	float gl = (float) ((float) deskCopy.co.galact[0] / (float) 86400000.0);
	float gb = (float) ((float) deskCopy.co.galact[1] / (float) 86400000.0);

	turn_deg(gl, tmp);
	memcpy(hdr->gl_blk, tmp, strlen(tmp));					// gl_blk = "ddd.dddd"
	turn_deg(gb, tmp);
	memcpy(hdr->gb_blk, tmp, strlen(tmp));					// gb_blk = "-dd.dddd"

	float az = (float) ((float) deskCopy.co.azzaa[0] / (float) 86400000.0);
	float ze = (float) ((float) deskCopy.co.azzaa[1] / (float) 86400000.0);

	
	turn_deg(az, tmp);
	memcpy(hdr->az_blk, tmp, strlen(tmp));					// az_blk = "ddd.dddd"
	turn_deg(ze, tmp);
	memcpy(hdr->zen_blk, tmp, strlen(tmp));					// zen_blk = "ddd.dddd"

	sprintf(tmp, "%4d", beam);
	memcpy(hdr->ibeam, tmp, strlen(tmp));

	return 0;
}

int turn_hms(double turn, char *hms)
{
	int hh, mm, isec;
	double sec;

	hh = (int) floor(turn*24.);
	mm = (int) floor((turn*24.-hh)*60.);
	sec = ((turn*24.-hh)*60.-mm)*60.;
	isec = (int) floor((sec*10000. + 0.5)/10000);
	if(isec == 60)
	{
		sec =0.;
		mm = mm + 1;
		hh = hh+1;
		if (mm == 60)
		{
			mm = 0;
			hh = hh+1;
			if(hh == 24)
			{
				hh = 0;
			}
		}
	}
	else if (mm == 60)
	{
		mm = 0;
		hh = hh+1;
		if(hh == 24)
		{
			hh = 0;
		}
	}
	else if (hh == 24)
	{
		hh = 0;
	}

	sprintf(hms, " %02d:%02d:%05.2f", hh%24, mm, sec);
	return 0;
}

int turn_dms(double turn, char *dms)
{
	// Converts double turn to string "sddd:mm:ss.sss"

	int dd, mm, isec;
	double trn, sec;
	char sign;

	sign = ' ';
	if(turn < 0.)
	{
		sign = '-';
		trn = -turn;
	}
	else
	{
		sign = '+';
		trn = turn;
	}
	dd = (int) floor(trn * 360.);
	mm = (int) floor((trn*360.-dd)*60.);
	sec = ((trn*360.-dd)*60.-mm)*60.;
	isec = (int) floor((sec*1000. + 0.5)/1000);
	if(isec==60)
	{
		sec=0.;
		mm=mm+1;
		if(mm==60)
		{
			mm=0;
			dd=dd+1;
		}
	}
	sprintf(dms, "%c%02d:%02d:%06.3f",sign,dd,mm,sec);
	return 0;
}

int turn_deg(double turn, char * deg)
{
	sprintf(deg,"%8.3f", turn*360.);
	return 0;
}
