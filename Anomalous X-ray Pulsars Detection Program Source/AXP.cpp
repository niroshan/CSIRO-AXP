// AXP.cpp : Defines the entry point for the application.
// Writen By Niroshan Rajadurai, Vacation Student (Summer 1998 - 1999)

#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 1000

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text



// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



// standard Main() ... only microsoft stnd for windows (GUI)  applics
// This function is very std....
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST4, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TEST4);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	return msg.wParam;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TEST4);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TEST4;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}



//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	int nXpos;
	int i;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDC_GREETINGS, szHello, MAX_LOADSTRING);
	int status;
	DWORD dwThreadId;
	static WORD wCurrentDC = IDC_VIEW_TIME;
	float freq;
	RECT MyRect;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:		// request menu item ABOUT
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About); // call function to handle dialog box
				   break;
				case IDM_EXIT:	// request by user to stop acquiring data
				   sh_mem.proc_cont.do_i_die = TRUE;				   // let all threads know they need to die...
				   MessageBox(hWnd, "Please wait while killing threads...", "Please Wait.....", MB_OK);
				   Sleep(5000);											// give the system time to kill any threads...
				   DestroyWindow(hWnd);
				   break;
				case IDC_NEW:	// Create a new acquisition
					sh_mem.hWnd = hWnd;									// pass window's handle to other threads
					sh_mem.ps = &ps;									// pass window's paint pallette to other threads
					sh_mem.proc_cont.do_i_die = TRUE;					// let other threads run
					sh_mem.proc_cont.print_ok = FALSE;					// Snychronization with graphics
					sh_mem.graphics.start_ch = 0;						// set def start chan to end chan to obs
					sh_mem.graphics.end_ch = 38;
					sh_mem.TapeHdr.block_num = 1;						// block start obv. 1
					GetClientRect(hWnd, &MyRect);						// window work space
					status = DialogBox(hInst, (LPCTSTR)IDD_SETTINGS, hWnd, (DLGPROC)GetSysSet); // get acq. settings
					if(!status) // cancel request
						break;
					// well if we got here, then inputs to above dialog were okay, or no cancel req.
					// so open the file, using binary file open....
					sh_mem.dat_file.fd = _open((char *) sh_mem.dat_file.path_to_sve, _O_APPEND | _O_CREAT | _O_BINARY | _O_RDWR );
					// now the file handle is open..... setup menu formatting....
					socket_talk(); // grab current deskcom status
					for(i=0; i < sh_mem.chn_stp.num_beams; i++)
					{		// fill hdr with no changing info.....
						fill_hdr_strt((struct tphdr*) &(sh_mem.TapeHdr.FileHdr[i]));
					}
					// disable and enable relevent menu items... prv. users from doing nasty things to us
					EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_GRAYED);
					CreateThread(NULL, 0, acq_data, NULL, 0, &dwThreadId);//this starts of the acquisition thread...
					break;
				case IDC_PAUSE: // pause acquiring... 
								// provision is here for this.. but not really recommended as not fully tested...
					// once again enable/dissable rel. menu items...
					EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_GRAYED);
					// kill threads.... we do this becaume the device driver for nat inst does not
					// let us pause their device.. doing so would result in it craching due to the halfbuffer not
					// being read.. blah, blah, blah..... just can't be done, so we just shut down the thread...
					sh_mem.proc_cont.do_i_die = TRUE;
					sh_mem.proc_cont.print_ok = FALSE;
					if(sh_mem.dat_file.fd == 0) // just incase we have someone trying to break our program....
												// u need to strt b4 u can do this.....
					{
						MessageBox(hWnd, "Session Not In Progress...", "User Error!!", MB_OK);
						EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_ENABLED);
						EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_ENABLED);
					}
					break;
				case IDC_RESUME: // resume acquiring
					// enable/dissble rel. menu items...
					EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_GRAYED);
					if(sh_mem.dat_file.fd != 0) // rewind the block counter back one, as we lost the block we were doing.....
					{
						sh_mem.TapeHdr.block_num--;
						CreateThread(NULL, 0, acq_data, NULL, 0, &dwThreadId);
					}
					else
					{	// users try to break us.... not a chance....
						MessageBox(hWnd, "Session Not In Progress...", "User Error!!", MB_OK);
						EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_ENABLED);
						EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_GRAYED);
						EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_ENABLED);
					}
					break;
				case IDC_VIEW_TIME: // view data in time domain
					sh_mem.graphics.time = TRUE; // flag paintprogram
					sh_mem.graphics.fft = FALSE;
					EnableMenuItem(GetMenu(hWnd), IDC_VIEW_TIME, MF_GRAYED); // dissable this option as it is in use
					EnableMenuItem(GetMenu(hWnd), IDC_VIEW_FFT, MF_ENABLED); // enable fft for switch
					InvalidateRect(hWnd, NULL, TRUE);						// ALWAYS bdo this b4 painting
					paintblock(hWnd);										// our GUI painter
					break;
				case IDC_VIEW_FFT: // view in Freq. domain
					sh_mem.graphics.time = FALSE;
					sh_mem.graphics.fft = TRUE; // flag paint prog.
					EnableMenuItem(GetMenu(hWnd), IDC_VIEW_TIME, MF_ENABLED); // dissbale/enable opts.
					EnableMenuItem(GetMenu(hWnd), IDC_VIEW_FFT, MF_GRAYED);
					InvalidateRect(hWnd, NULL, TRUE);
					paintblock(hWnd);
					break;
				case IDC_STOP:
					sh_mem.proc_cont.do_i_die = TRUE;		// shut down acq
					sh_mem.proc_cont.print_ok = FALSE;
					EnableMenuItem(GetMenu(hWnd), IDC_NEW, MF_ENABLED);
					EnableMenuItem(GetMenu(hWnd), IDC_PAUSE, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_RESUME, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDC_STOP, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDM_EXIT, MF_ENABLED);
					_commit(sh_mem.dat_file.fd); // flush to file.....
					_close(sh_mem.dat_file.fd);  // close file
					sh_mem.dat_file.fd = 0;		 // reset file handle
					break;
				case IDC_BEAMS:					// sleect beams to view...
					status = DialogBox(hInst, (LPCTSTR)IDD_BEAMS2SHOW, hWnd, (DLGPROC)GetBeam); // dlg proc. to handle this
					if(!status) // cancel request
						break;
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_RBUTTONDOWN:  // refresh screen , graph....
			InvalidateRect(hWnd, NULL, TRUE);
			paintblock(hWnd);
			break;
		case WM_LBUTTONDOWN:  // for fft to find out freqs...
			if (sh_mem.graphics.fft == TRUE)
			{
				nXpos = LOWORD(lParam);		
				GetClientRect(hWnd, &MyRect);
				freq  = (((float) nXpos / (float) MyRect.right) * (float) 1048.0) * (float) ( (float) ((float) sh_mem.smp_prds.strg/(float) 2.0) / (float) 1048.0);
				sprintf(szHello, "Freq = %f Hz ", freq);
				MessageBox(hWnd, szHello, "The Frequency is.....", MB_OK);
			}
			break;
		case WM_TIMER:		// didn't work prop. but left should impl. be dev. latter
			break;
		case WM_PAINT:		// the widnow needs to be repaintd due to : chng size, another prog. on top of.....
							// this is the std refresh call by the wind.. we do not need to innld. rect. here only
			paintblock(hWnd);
			break;
		case WM_DESTROY:	// go bye bye.....
			sh_mem.proc_cont.do_i_die = TRUE;				   
			Sleep(5000);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}






// Mesage handler for about box.
// ABOUT BOX......
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

