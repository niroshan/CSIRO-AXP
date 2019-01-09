#include "StdAfx.h"
#include "resource.h"



#define DEF_SMP_ACQ		"1000";
#define DEF_SMP_STR		"50";
#define DEF_CH_BW		"100.00";
#define DEF_CH_CTR		"1000.00";
#define	DEF_NUM_CHN		"3"
#define DEF_NUM_BMS		"13"
#define DEF_SCK_ADD		"scutum.atnf.csiro.au"
#define	DEF_SCK_PRT		"4005"

volatile SET_VAR sh_mem;

LRESULT APIENTRY GetSysSet( HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	char szBuff[80] = {0};
	switch(message) 
	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hDlg, IDC_SAMP_STR));
			SetWindowText(GetDlgItem(hDlg, IDC_SAMP_STR), "50");
			SetWindowText(GetDlgItem(hDlg, IDC_SAMP_ACQ), "1000");
			SetWindowText(GetDlgItem(hDlg, IDC_CH1_CNTR_FREQ), "1000.00");
			SetWindowText(GetDlgItem(hDlg, IDC_CH1_BW), "100.00");
			SetWindowText(GetDlgItem(hDlg, IDC_CHAN_PER_BEAM), "3");
			SetWindowText(GetDlgItem(hDlg, IDC_BEAMS), "13");
			SetWindowText(GetDlgItem(hDlg, IDC_HST_NME), "scutum.atnf.csiro.au");
			SetWindowText(GetDlgItem(hDlg, IDC_PORT_ADD), "4005");
			SetWindowText(GetDlgItem(hDlg, IDC_BITS_PER_SAMPLE_OUT), "8");
			SetWindowText(GetDlgItem(hDlg, IDC_FILE_NAME), "file_name.xdt");
			return (TRUE);

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case IDOK:
						{
							// Get sample store rate
							GetDlgItemText(hDlg, IDC_SAMP_STR, szBuff, 10);
							if ((sh_mem.smp_prds.strg = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								int mytmp = sh_mem.smp_prds.strg;
								if((mytmp != 50) && (mytmp != 25) && (mytmp != 20) && (mytmp != 15) && (mytmp != 10))
								{
									MessageBox(hDlg, "Sample store can only be 50, 25, 20, 15, 10", "ERROR", MB_OK);
									break;
								}
							}
							// Get ch1 center freq
							GetDlgItemText(hDlg, IDC_CH1_CNTR_FREQ, szBuff, 30);
							if ((sh_mem.chn_stp.ch1_cntr_freq = (float) atof(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								if (sh_mem.chn_stp.ch1_cntr_freq <= 0)
								{
									MessageBox(hDlg, "Ch1 Cntr Freq MUST be >= 0", "ERROR", MB_OK);
									break;
								}
							}
							// get channels per beam
							GetDlgItemText(hDlg, IDC_CHAN_PER_BEAM, szBuff, 10);
							if ((sh_mem.chn_stp.num_chan_per_beam = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								if (sh_mem.chn_stp.num_chan_per_beam <= 0)
								{
									MessageBox(hDlg, "Chans Per Beam needs to be >= 0", "ERROR", MB_OK);
									break;
								}
							}
							// get num beams
							GetDlgItemText(hDlg, IDC_BEAMS, szBuff, 10);
							if ((sh_mem.chn_stp.num_beams = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								if ((sh_mem.chn_stp.num_beams <= 0) || (sh_mem.chn_stp.num_beams > 13))
								{
									MessageBox(hDlg, "Number Beam needs to be >= 0, or <= 13", "ERROR", MB_OK);
									break;
								}
							}
							// get channel 1 band width
							GetDlgItemText(hDlg, IDC_CH1_BW, szBuff, 50);
							if ((sh_mem.chn_stp.ch1_bw = (float) atof(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								if ((sh_mem.chn_stp.ch1_bw >= sh_mem.chn_stp.ch1_cntr_freq))
								{
									MessageBox(hDlg, "Chn BW cannot be >= Chan Cntr Freq", "ERROR", MB_OK);
									break;
								}
							}
							// get bits per sample out
							GetDlgItemText(hDlg, IDC_BITS_PER_SAMPLE_OUT, szBuff, 50);
							if ((sh_mem.chn_stp.bits_sample_out = atoi (szBuff)) == 0)
								EndDialog(hDlg, 2);
							// get sample acquire rate (from nat inst device)
							GetDlgItemText(hDlg, IDC_SAMP_ACQ, szBuff, 50);							
							if ((sh_mem.smp_prds.dacq = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							GetDlgItemText(hDlg, IDC_HST_NME, szBuff, 100);							
							if (atoi(szBuff) == 0)
								EndDialog(hDlg, 2);
							else
							{
								strcpy((char *) sh_mem.sck.host_name, szBuff);
							}
							GetDlgItemText(hDlg, IDC_PORT_ADD, szBuff, 50);							
							if ((sh_mem.sck.port_add = atoi (szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								if ((sh_mem.sck.port_add <1000) || (sh_mem.sck.port_add > 9999))
								{
									MessageBox(hDlg, "Invalid port address", "ERROR", MB_OK);
									break;
								}
							}
							// get the file name.............
							GetDlgItemText(hDlg, IDC_FILE_NAME, szBuff, 150);
							sprintf((char *) sh_mem.dat_file.path_to_sve, "d:\\Users\\Pulsar\\AXPData\\%s", szBuff);
							sh_mem.dat_file.valid_name = TRUE;
							EndDialog(hDlg, 1);
							break;
						}
					case IDCANCEL:
						{
							EndDialog(hDlg, 0);
							break;
						}
					default:
						return FALSE;
				}
				return (TRUE);
			}
	}
	return (FALSE);
	UNREFERENCED_PARAMETER(lParam);
}

LRESULT APIENTRY GetBeam(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	char szBuff[80] = {0};
	switch(message) 
	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hDlg, IDC_START));
			SetWindowText(GetDlgItem(hDlg, IDC_START), "1");
			SetWindowText(GetDlgItem(hDlg, IDC_END), "13");
			return (TRUE);

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case IDOK:
						{
							int start, end;
							GetDlgItemText(hDlg, IDC_START, szBuff, 10);
							if ((start = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								int mytmp = start;
								if((mytmp <=0) || (mytmp > 13))
								{
									MessageBox(hDlg, "Beam 1 to 13 only available", "ERROR", MB_OK);
									break;
								}
								else
									sh_mem.graphics.start_ch = (start - 1) * 3;
							}
							GetDlgItemText(hDlg, IDC_END, szBuff, 10);
							if ((end = atoi(szBuff)) == 0)
								EndDialog(hDlg, 2);
							else
							{
								int mytmp = end;
								if((mytmp <=0) || (mytmp > 13) || (mytmp < start))
								{
									MessageBox(hDlg, "Beam 1 to 13 only available, or greater than starting beam", "ERROR", MB_OK);
									break;
								}
								else
									sh_mem.graphics.end_ch = (end * 3) - 1;
							}
							EndDialog(hDlg, 1);
							break;
						}
					default:
						return FALSE;
				}
				return (TRUE);
			}
	}
	return (FALSE);
	UNREFERENCED_PARAMETER(lParam);
}
