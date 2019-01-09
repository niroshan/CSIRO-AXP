/*  Desk message definitions file  */

/****   see  deskcom_inc on yowie  ****/

/**  This version for the SUN is in UNPACKED mode;  that is,
     in remotemon the buffer is memcpy'd in sections to maintain
     the alignment.
     The problem arises because although some effort was made
     to resolve the alignment to I*2 boundaires, the SUN works in
     I*4.
     In the comments section the byte count and the original
     packed addressing is shown.
     a blank line signals the start of each memcpy section.

     **/

/*  message length definitions */
#ifndef DESK_STRUCT
#define DESK_STRUCT

#define CORXLN   212
#define COTXLN    60
#define SERXLN    60
#define SETXLN    20
#define MERXLN    24
#define METXLN    18
#define LNCRXLN  120
#define LNCTXLN   80
#define LNDRXLN   13
#define LNDTXLN  250
#define MSGHDR     2    /* 2-byte message header */
#define MSGTLR     1    /* 1 byte checksum trailer */
#define FCCXLN   294

/*
 *      fcc.h ..
 *
 */


/*  This is correctly aligned - start at buff[272] and copy 292 bytes. */
#pragma pack( push, 1 )
struct fccTag
{
	float z;                /* axis position */
	float y1;
	float y2;
	float r1;
	float r2;
	float spare[20];
	unsigned int  fccError;
	unsigned int  trkError;
	char  rxName[16];
	char  trk_mode[8];
	char  mZ[8];           /* motor status */
	char  mY1[8];
	char  mY2[8];
	char  mR1[8];
	char  mR2[8];
	char  fcc_err_msg[60];
	char  trk_err_msg[60];
};
#pragma pack( pop, 1 )	 

/*
*               coord.h  ...
*
*  mjk, 9/10/96
*
*/

#define CRD     1
#define SRV     2


/*              coord states  */
#ifndef COINSIDE
#define COIDLE  81
#define COFAIL  82
#define COGO    83 
#define COSCAN  84
#define COUP    85
#define CODOWN  86
#define CODRV   87
#define CODONE  88

#endif

/*              ME states       */

#define MESTOP 121              /* normal codes */
#define MESLEW 122
#define METRK  123 
#define MESLHP 125              /* soft limit codes */
#define MESLHM 126 
#define MESLDP 127  
#define MESLDM 128 
#define MEHLHP 130              /* hard limit codes */
#define MEHLHM 131 
#define MEHLDP 132  
#define MEHLDM 133 
#define MELOCL 135              /* in local control             */
#define MENOS  136              /* error - no 1-sec tick        */
#define MENODM 137              /* error - no demand position   */
#define MENOEN 138              /* error - encoder err          */
#define MEFAIL 139              /* error - bad dump             */
 
 

/*  coord mode definitions  
*                       1       RA,Dec   Besellian year date
*                       2       Galactic
*                       3       RA,Dec   B1950.0
*                       4       RA,Dec   Julian year date
*                       5       RA,Dec   J2000.0
*                       6       RA,Dec   rel to current apparent equ & equ
*                       7       HA,Dec
*                       8       Az,Za
*                       9       User specified
*/

/*
  The transitions :

  .co.coctrl[0]  buff[0]    6 bytes
  .co.haapp      buff[6]   74
  .co.byear      buff[80]  86
  .co.beampo[0]  buff[166] 38
  .co.cobat[0]   buff[204]  8

  */

#pragma pack( push, 1 )
struct coordTag {               /* dump buffer format  */
	char  coctrl[2];        /* 0 2 control system specific data */
	char  costat;           /* 2 1 coord status */
	char  cobeam;           /* 3 1  .... obsolete */
	short comode;           /* 4 2 6 coordinate mode */

	long  haapp;            /* 6 4 ha/dec - date   */
	long  radeca[2];        /* 10 8 ra/dec - date   */
	long  azzaa[2];         /* 18 8 true az/ze */
	long  azzac[2];         /* 26 8 requested az/ze */
	long  radec5[2];        /* 34 8 J2000        */
	long  radec4[2];        /* 42 8 B1950        */
	long  galact[2];        /* 50 8 galactic     */
	long  userco[2];        /* 58 8 user defined */
	long  q;                /* 66 4 parallactic angle */
	long  tileft;           /* 70 4 time left on source */
	long  coutc;            /* 74 4 UT time stamp for the coordinates */
	short reqack;           /* 78 2 74 request ACK  */

	float byear;            /* 80 4 Besselian date */
	float jyear;            /* 84 4 Julian date    */
	float copnt[11];        /* 88 44 the pointing parameters */
	long  usrcoo[7];        /* 132 28 user defined coordinates */
	long  coplax;           /* 160 4 object's parallax */
	short spare;            /* 164 2 86  */

	long  beampo[2];        /* 166 8 beam number/position */
	short trgsta;           /* 174 2 trigger state */
	short trgint;           /* 176 2 trigger interval */
	short trgnch;           /* 178 2 trigger channel  */
	char  mectrl[2];        /* 180 2 ME section ;  fn, count     */
	short merate[2];        /* 182 4 in HA, DEC                  */
	long  mespar;           /* 186 4    */
	long  meposn[2];        /* 190 8  HA, DEC (encoder units)     */
	long  meerr;            /* 298 4 error vector magnitude  (asec) */
	char  mestat;           /* 202 1 ME status  */
        char  mefill;           /* 203 1  38 */

	short cobat[4];         /* 204 8 8 BAT time from coord          */
};
#pragma pack( pop, 1 )

/*      message format          */

#pragma pack( push, 1 )
struct coordMsgTag
{
	unsigned char bcount;
	char id;

	char cmd;
	unsigned char ccount;
	int utspec;
	char body[COTXLN-9];

	unsigned char check;
};
#pragma pack( pop, 1 )

/*
*       servo.h  ... definitions file 
* 
*  based on sedmp.inc
*
* mjk, 9/10/96
*
*/

/*                      Servo states            */

#define SEIDLE  161             /* idle         */
#define SESTOP  162             /* stopped      */
#define SEDRVG  163             /* driving      */
#define SEPOSN  164             /* in position  */
#define SELCKG  165             /* locking      ?       */
#define SELOCK  166             /* locked       */
#define SEFAIL  167             /* fail         ?       */
#define SEBRAK  168             /* brakes on    ?       */
#define SEHWFL  169             /* HW failure   */
#define SERSET  170             /* reset        ?       */
#define SESTPG  171             /* stopping     ?       */
#define SEWSTW  172             /* Wind Stow            */


/*  transitions :

  .se.sectrl[0]       buff[212]    2 bytes
  .se.seaz            buff[214]   14
  .se.seermx[0]       buff[228]   44

  */

#pragma pack( push, 1 )
struct servoTag {               /* dump buffer definition */
	char  sectrl[2];  

	long  seaz;                     /* servo az (encoder units) */
	long  seza;                     /* servo ze (encoder units) */
	short azavsp;                   /* az rate */
	short zeavsp;                   /* el rate */
	char  sector;                   /* sector [1 - East; 2 - West] */
	char  sestat;                   /* servo status  */

	short seermx[2];                /* max error (az and ze) in asec */
	char  sehwst[8];        /* hardware status */
	char  sewind;                   /* wind velocity (km/hr) */
	char  sewdir;                   /* wind direction */
	char  seconf;                   /* drive status   */
	char  space1;
	short seazr;
	short sezar;
	long  seazl;
	long  sezal;
	short seerav[2];
	short azm1cu;           /* motor currents */
	short azm2cu;
	short zem1cu;
	short zem2cu;
	short space2;
	short space3;
};
#pragma pack( pop, 1 )

/*    servo message format   */

#pragma pack( push, 1 )
struct servoMsgTag
{
	unsigned char bcount;
	char id;

	unsigned char cmd;
	char ccount;
	char body[SETXLN-5];
	char check;
};
#pragma pack( pop, 1 )
 
/* ME message definitions */

#pragma pack( push, 1 )
struct meMsgTag {
	unsigned char bcount;
	char id;
	unsigned char cmd;
	char ccount;
	char body[METXLN-5];
	char check;
};
#pragma pack( pop, 1 )

/*  DESK state definitions */

#define DESKIDLE  1
#define GOWAIT    2
#define GOSLEW    3
#define GOLOCK    4


/*  DUMP format         */

#pragma pack( push, 1 )
struct deskTag
{
	struct coordTag co;
	struct servoTag se;
	struct fccTag  fcc;
	char dummy[100];
};
#pragma pack( pop, 1 )
/*  message definitions                 */

struct lastCmd
{
	char tim[12];
	char cmd[6];
	int  flag;
};

/*   Message function codes  */

#define MFGO     64     /* 0. go   */
#define MFDRIV   65     /* 1. drive under ME lock  */
#define MFSCSC   80     /* 2. small circle scan    */
#define MFSCGC   81     /* 3. great circle scan    */
#define MFCHAI   96     /* 4. chain ??             */
#define MFSET   128     /* 5. parameter set        */
#define MFSTOP  144     /* 6. stop                 */
#define MFDUMP  145     /* 7. dump                 */

#define MFFR    151     /* feed rotation        */
#define MFFRL   152     /* feed rotation limits */
#define MFFF    153     /* feed focus position  */
#define MFFL    154     /* feed focus limits    */
#define MFFST   155     /* feed stop all motion */
#define MFFPA   156     /* feed start paralactic */

#define MFMEDU   32     /* ME dump block        */
#define MFCODU   33     /* coord dump block     */
#define MFFEDU   34     /* feed dump block      */
#define MFDIDU   35     /* dish dump block      */
#define MFCSDU   36     /* coord special dump   */
#define MFALDU   37     /* full dump to the LAN */

#define MFMKLG   42     /* make event log into file */
#define MFME     44     /* ME parameter request */


#define MCSTDI  1               /* stop dish  */
#define MCSTME  2               /* stop ME    */
#define MCSTMD  3               /* stop both  */
#define MCSTFE  4               
#define MCSTFD  5
#define MCSTFM  6
#define MCSTAL  7               /* stop everything */

				/* SET qualifiers  */
#define MCMEON  1               /* turn ON the ME  */
#define MCAZON  2
#define MCZAON  4
#define MCALK   8
#define MCFEON 16
				/* message offsets (start at 0, not 1) */
#define MSGFN    0
#define MSGCNT   1
#define MSGTIM   2
#define MSGSTP   6
#define MSGMOD   6
#define MSGCO1   7
#define MSGCO2  11
#define MSGRAT  15
#define MSGE1   19
#define MSGE2   23
#define MSGENT   6
#define MSGVAL  12
#define MSGPOS   2
#define MSGLM1   4
#define MSGLM2   6
#define MSGFAR   8
#define MSGPAL   6
#define MSGID    2
#define MSGRSN   3

/*----------------------------------------------------*/

#ifdef ROOT

struct deskTag     deskCom;
struct coordMsgTag coordMsg;            /* command sent to Coord */
struct servoMsgTag servoMsg;            /*                 Servo */
struct meMsgTag    meMsg;               /*                 ME    */
struct lastCmd     coordCmd;            /* display string */
struct lastCmd     servoCmd;
				/* config states */
char   config;
int    autolk=1;
int    meon=1;
int    dishon=1;
int    feon=0;
	
char *servoTxt[13] = 
     {"incorrect state code",
      "idle                ",
      "stopped             ",
      "driving             ",
      "in position         ",
      "locking             ",
      "locked              ",
      "fail                ",
      "brakes on           ",
      "HW failure          ",
      "reset               ",
      "stopping            ",
      "wind park           "
     };

char *coordTxt[9] =
    { "incorrect state code",
      "idle                ",
      "fail                ",
      "go                  ",
      "scan                ",
      "ramp up             ",
      "ramp down           ",
      "driving             ",
      "done                "
    };

char *meTxt[20] = 
    { "incorrect state code        ",
      "stop                        ",
      "slewing                     ",
      "tracking                    ",
      "undefined code (124)        ",
      "soft limit - HP             ",
      "soft limit - HM             ",
      "soft limit - DP             ",
      "soft limit - DM             ",
      "undefined code (129)        ",
      "hard limit - HP             ",
      "hard limit - HM             ",
      "hard limit - DP             ",
      "hard limit - DM             ",
      "undefined code (130)        ",
      "local mode                  ",
      "error - no 1-sec            ",
      "error - no demanded position",
      "error - encoder error       ",
      "error - bad dump            "
    };


char *cnfig  = "config";
char *bdate  = "bdate ";
char *jdate  = "jdate ";
char *pntcor = "pntcor";
char *marker = "marker";
char *evntrg = "evntrg";
char *beamdf = "beamdf";
char *beam   = "beam  ";
char *usersp = "usersp";
char *overlp = "overlp";
char *spdump = "spdump";
char *stow   = "stow  ";
char *park   = "park  ";
char *pin    = "pin   ";
char *brak   = "break ";
char *lock   = "lock  ";
char *disabl = "disabl";
char *enable = "enable";
char *fectrl = "fectrl";
char *parlax = "parlax";

	 
#else

extern struct deskTag     deskCom;
extern struct coordMsgTag coordMsg;
extern struct servoMsgTag servoMsg;
extern struct meMsgTag    meMsg;
extern struct lastCmd     coordCmd;             /* display string */
extern struct lastCmd     servoCmd;

extern char   config;
extern int    autolk;
extern int    meon;
extern int    dishon;
extern int    feon;

extern char *servoTxt[12];
extern char *coordTxt[9];    
extern char *meTxt[20];

extern char *cnfig ;
extern char *bdate ;
extern char *jdate ;
extern char *pntcor;
extern char *marker;
extern char *evntrg;
extern char *beamdf;
extern char *beam  ;
extern char *usersp;
extern char *overlp;
extern char *spdump;
extern char *stow  ;
extern char *park  ;
extern char *pin   ;
extern char *brak  ;
extern char *lock  ;
extern char *disabl;
extern char *enable;
extern char *fectrl;
extern char *parlax;

#endif



#endif