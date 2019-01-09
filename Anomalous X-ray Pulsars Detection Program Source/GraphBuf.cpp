#include "stdafx.h"
#include "resource.h"




void fft_print_array (const char *msg, unsigned long nn, float data[]);


#define SHIFT 60
void paintblock(HWND hWnd)
{
	RECT rect;
	HBRUSH hbrush;
	HPEN hPen;
	POINT Pos[DATA_BLOCKS];
	PAINTSTRUCT myps;
	HDC hdc;
	int bottom, shiftup =  60;
	int i, j, k;
	static short nRuleSize[10] = {	90, 16, 36, 36, 18, 54, 18, 36, 36, 18};

	
	hdc = BeginPaint(hWnd, &myps);

	SetMapMode(hdc, MM_ANISOTROPIC);
	GetClientRect (hWnd, &rect);
	hbrush = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hdc, hbrush);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(SelectObject(hdc, hbrush));
	hPen = CreatePen(PS_SOLID, 1, RGB(250, 250, 250));
	SelectObject(hdc, hPen);
	bottom = rect.bottom - shiftup;
	MoveToEx(hdc, rect.left, bottom, NULL);
	LineTo(hdc, rect.right, bottom);
	MoveToEx(hdc, rect.left, 2 * bottom/3, NULL);
	LineTo(hdc, rect.right, 2 * bottom/3);
	MoveToEx(hdc, rect.left, 1 * bottom/3, NULL);
	LineTo(hdc, rect.right, 1 * bottom/3);


	char sline[200];
	SetBkMode (hdc,TRANSPARENT); 
	SetTextColor(hdc, RGB(0, 255, 0));
	int blck = sh_mem.TapeHdr.block_num - 1;
	int buf = bufpos - 1;
	/*
	if((buf == -1) && (blck > 0))
	{
		buf = 5;
		blck = sh_mem.TapeHdr.block_num - 1;
	}*/
	if ((buf == -1) && (blck == 0))
	{
		buf = 0;
	}


	
	if ((sh_mem.graphics.fft == TRUE) || (sh_mem.graphics.time == TRUE))
	{
		if ((sh_mem.smp_prds.strg > 0) && (sh_mem.smp_prds.strg <= 50))
		{
			j=10;
			for(i=0; i < rect.right; i =i + (rect.right - rect.left)/10)
			{
				SetTextColor(hdc, RGB(0, 255, 0));
				if (sh_mem.graphics.fft == TRUE)
					sprintf (sline , "%2.2f", (float) j * ((float) sh_mem.smp_prds.strg / 2) / (float) 10);
				else if (sh_mem.graphics.time == TRUE)
					sprintf (sline , "%2.2f", (((float) j * (float) (sh_mem.smp_prds.dacq / sh_mem.smp_prds.strg) * (float) DATA_BLOCKS) / (float) 10000.0) + (float) (blck * 6 + buf) * (((float) (sh_mem.smp_prds.dacq / sh_mem.smp_prds.strg) * (float) DATA_BLOCKS) / (float) 1000.0));
				TextOut(hdc, rect.right  - i, rect.bottom - 40, sline, strlen(sline));
				j--;
			}
		}

		if (sh_mem.graphics.fft == TRUE)
			strcpy (sline , "Frequency (Hz)");
		else 
			strcpy (sline , "Time (Sec)");

		TextOut(hdc, rect.right - 500, rect.bottom - 20, sline, strlen(sline));
	}
	else
	{
		SetTextColor(hdc, RGB(0, 255, 0));
		sprintf(sline, "Please start the program, and choose how to view data");
		TextOut(hdc, rect.right  - 700, rect.bottom - 40, sline, strlen(sline));
	}

	SetTextColor(hdc, RGB(255, 255, 255));
	
	if(sh_mem.graphics.fft == TRUE)
		sprintf(sline,"FFT Plot, %dHz Sampling", sh_mem.smp_prds.strg);
	else if (sh_mem.graphics.time == TRUE)
		sprintf(sline,"Time Domain Plot, %dHz Sampling", sh_mem.smp_prds.strg);
	else
		sprintf(sline,"The Search For Anomalous X-ray Pulsars In The Radio Frequencies");

	SetWindowText(hWnd, sline);
	
	DrawText(hdc, sline, strlen(sline), &rect, DT_CENTER);	
	


	// graph data now.................
	SetViewportOrgEx(hdc, rect.right, rect.bottom, NULL);
	MoveToEx(hdc, -rect.right, 0,NULL);


	// Draw the ruler
	hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
	SelectObject(hdc, hPen);

	for(i=0; i < rect.right; i =i + (rect.right - rect.left)/10)
	{
		MoveToEx(hdc, -rect.right + i, rect.top, NULL);
		LineTo(hdc, -rect.right  + i, rect.top - rect.bottom);
	}
	//end drawing ruler...........



	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	SelectObject(hdc, hPen);
	if (sh_mem.graphics.end_ch > sh_mem.chn_stp.num_beams * sh_mem.chn_stp.num_chan_per_beam)
		sh_mem.graphics.end_ch = sh_mem.chn_stp.num_beams * sh_mem.chn_stp.num_chan_per_beam;
	if( sh_mem.graphics.start_ch < 0)
		sh_mem.graphics.start_ch = 0;
	
	int mybuf[39][DATA_BLOCKS];
	int mynewbot;
	memcpy(mybuf, (int *) sh_mem.acq.GoodBuf, sizeof(int) * 39 * DATA_BLOCKS);
	if (sh_mem.graphics.time == TRUE)
	{
		sprintf(sline,"Time Domain Plot");
		SetWindowText(hWnd, sline);
		
		for(k=sh_mem.graphics.start_ch; k <= sh_mem.graphics.end_ch; k++)
		{

			int RGB_CONST = 255 / 13;
			if(k <= 2) // Beam 1
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 13, RGB_CONST * 7, RGB_CONST * 7));
			}
			else if (k <= 5) // Beam 2
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 4, RGB_CONST * 7, RGB_CONST));
			}
			else if (k <= 8)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 8, RGB_CONST * 7, RGB_CONST));
			}
			else if(k <= 11)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 13, RGB_CONST * 7, RGB_CONST));
			}
			else if(k <= 14)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST * 13, RGB_CONST * 7));
			}
			else if(k <= 17)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 4, RGB_CONST * 7));
			}
			else if(k <= 20)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 8, RGB_CONST * 7));
			}
			else if(k <= 23)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 13, RGB_CONST * 7));
			}
			else if(k <= 26)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST  * 7, RGB_CONST * 13));
			}
			else if(k <= 29)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST* 7, RGB_CONST, RGB_CONST * 4));
			}
			else if(k <= 32)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST, RGB_CONST * 8));
			}
			else if(k <= 35)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST, RGB_CONST * 13));
			}
			else if(k <= 38)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST * 7, RGB_CONST * 7));
			}
			else
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			}

			if( (k % 3) == 0)
			{
				mynewbot = 0;
			}
			else if ((k % 3) == 1)
			{
				mynewbot = 1 * (rect.bottom - SHIFT)/3;
			}
			else
			{
				mynewbot =2 * (rect.bottom - SHIFT)/3;
			}


			SelectObject(hdc, hPen);
			for(i=0;i<DATA_BLOCKS;i++)
			{
				//Pos[i].x = (int) -floor((LONG)  (rect.right * ((DATA_BLOCKS - i)/DATA_BLOCKS ))); // i add 40 to align this with the ruler
				//Pos[i].y = (-(shiftup+mynewbot)) + (int) -floor((rect.bottom * (LONG)  95 / (LONG)  100 *  (LONG)  mybuf[k][i])/ (LONG)  256 ) / 3;
			}
			Polyline(hdc, Pos, DATA_BLOCKS);
		}
	}
	else if (sh_mem.graphics.fft == TRUE)
	{
		float big, scal, *size, * data;
		data = vector(1, DATA_BLOCKS);
		size = vector(1, (DATA_BLOCKS)/2 + 1);
		int n = (DATA_BLOCKS)/2, nlim;


		MoveToEx(hdc, rect.left, 1 * bottom/4, NULL);
		sprintf(sline,"Frequency Domain Plot               -           AMPLITUDE (Norm.) V's Frequency (Hz)");
		SetWindowText(hWnd, sline);
	
		sprintf(sline,"\nAMPLITUDE (Norm.) V's Frequency (Hz)");
		DrawText(hdc, sline, strlen(sline), &rect, DT_CENTER);
		
		float mybig = 0.0;
		for(k=sh_mem.graphics.end_ch; k >= sh_mem.graphics.start_ch; k--)
		{
			int RGB_CONST = 255 / 13;
			if(k <= 2) // Beam 1
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 13, RGB_CONST * 7, RGB_CONST * 7));
			}
			else if (k <= 5) // Beam 2
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 4, RGB_CONST * 7, RGB_CONST));
			}
			else if (k <= 8)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 8, RGB_CONST * 7, RGB_CONST));
			}
			else if(k <= 11)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 13, RGB_CONST * 7, RGB_CONST));
			}
			else if(k <= 14)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST * 13, RGB_CONST * 7));
			}
			else if(k <= 17)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 4, RGB_CONST * 7));
			}
			else if(k <= 20)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 8, RGB_CONST * 7));
			}
			else if(k <= 23)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST, RGB_CONST * 13, RGB_CONST * 7));
			}
			else if(k <= 26)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST  * 7, RGB_CONST * 13));
			}
			else if(k <= 29)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST* 7, RGB_CONST, RGB_CONST * 4));
			}
			else if(k <= 32)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST, RGB_CONST * 8));
			}
			else if(k <= 35)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST, RGB_CONST * 13));
			}
			else if(k <= 38)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(RGB_CONST * 7, RGB_CONST * 7, RGB_CONST * 7));
			}
			else
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			}

			if( (k % 3) == 0)
			{
				mynewbot = 0;
			}
			else if ((k % 3) == 1)
			{
				mynewbot = 1 * bottom/3;
			}
			else
			{
				mynewbot =2 * bottom/3;
			}


			SelectObject(hdc, hPen);
			for(i=0; i < DATA_BLOCKS; i++)
			{
				data[i+1] = (float) mybuf[k][i];	
			}
			realft(data, DATA_BLOCKS, 1);
			//fft_print_array ("FFT", DATA_BLOCKS, data); //debugging stuff
			big = -1.0e10;
			for(i=2; i <=n;i++) {
				size[i] = (float) sqrt(SQR(data[2*i-1])+SQR(data[2*i]));
				if (size[i] > big) big=size[i];
			}
			size[1]= (float) fabs(data[i]);
			if (size[1] > big) big=size[1];
			size[n+1] = (float) fabs(data[2]);
			if(size[n+1] > big) big = size[n+1];
			if (mybig < big)
				mybig = big;
			scal=rect.bottom/big;
			for(i=1;i<=n;i++) 
			{
				float nlim_tmp;
				nlim_tmp =(int) (0.5+scal*size[i]+1.0e-2)/(float)3.0;
				nlim = (int) floor(nlim_tmp);
				Pos[i].x = (int)( (long) -1 * floor( rect.right * (DATA_BLOCKS/2 - i)/(DATA_BLOCKS/2) ));
				Pos[i].y = (-(shiftup+mynewbot)) -nlim;
			}


			Polyline(hdc, Pos, n);
		}
	}
	DeleteObject(hPen);
	EndPaint(hWnd, &myps);
}



///////////////////////////////////////////////////////////NUMERICAL RECEIPES/////////////////////////////////////////

void four1(float data[], unsigned long nn, int isign)
{
   /* four1() - Numerical Recipes */

	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=(float) wr*data[j]- (float)wi*data[j+1];
				tempi=(float) wr*data[j+1]+(float) wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

/* (C) Copr. 1986-92 Numerical Recipes Software #p21E6W)1.1&iE10(9p#. */


void realft(float data[], unsigned long n, int isign)
{
	void four1(float data[], unsigned long nn, int isign);
	unsigned long i,i1,i2,i3,i4,np3;
	float c1=0.5,c2,h1r,h1i,h2r,h2i;
	double wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793/(double) (n>>1);
	if (isign == 1) {
		c2 = -0.5;
		four1(data,n>>1,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	np3=n+3;
	for (i=2;i<=(n>>2);i++) {
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=(float) h1r+(float) wr*h2r-(float) wi*h2i;
		data[i2]=(float) h1i+(float) wr*h2i+(float) wi*h2r;
		data[i3]=(float) h1r- (float)wr*h2r+(float)wi*h2i;
		data[i4] =(float) -h1i+(float)wr*h2i+(float)wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		four1(data,n>>1,-1);
	}
}
/* (C) Copr. 1986-92 Numerical Recipes Software #p21E6W)1.1&iE10(9p#. */



float * vector(long nl, long nh)
{
   float *v;

   v=(float *) malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
   if (!v) fprintf(stderr, "allocation failure in vector ()\n");
   return (v-nl+NR_END);
}

void free_vector(float *v, long nl, long nh)
{
   free((FREE_ARG) (v+nl-NR_END));
}



/* (C) Copr. 1986-92 Numerical Recipes Software #p21E6W)1.1&iE10(9p#. */
/* ---------------------- end-of-file (c source) ---------------------- */


void fft_print_array (const char *msg, unsigned long nn, float data[])
{
   unsigned long n;
   float temp = 0.0;
   FILE * fptr;
   fptr = fopen("data_filt.dat", "w");


   if (msg && *msg) fprintf( fptr, "%s\n", msg);

   fprintf(fptr, "%4s %13s %13s %12s \n",
         "n", "real(n)", "imag.(n)", "Abs(n)");
   fprintf((FILE *) fptr, "   0 %14.6f %12.6f \n", 
         data[1], data[2]);
   for ( n = 3; n < nn; n +=2)
   {
      temp = data[n]*data[n] + data[n+1]*data[n+1];
      fprintf(fptr, "%4lu %14.6f %12.6f %12.6f \n",
            ((n-1)/2), data[n], data[n+1], sqrt(temp));
   }
   fprintf( (FILE *) fptr, "\n");
   fclose(fptr);

}