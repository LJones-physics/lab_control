//
// EasyProjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyProj.h"
#include "EasyProjDlg.h"
#include "afxdialogex.h"

#include <memory>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

GDIPLUS_APP;									// start GDI+

// CEasyProjDlg dialog

CEasyProjDlg::CEasyProjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEasyProjDlg::IDD, pParent)
	, m_ProjectorSerial(_T(""))
	, m_SequenceLoadedImages(_T(""))
	, m_iProjType(0)
	, m_ColourComponents(1)
	, m_iLedBrightness(0)
	, m_iLEDType(0)
	, m_SequenceBitnum(_T(""))
	, m_Led1Temperature(_T(""))
	, m_Led2Temperature(_T(""))
	, m_Led3Temperature(_T(""))
	, m_LedEnableMode( GATE_LOW )
	, m_PictureTime(0)
	, m_LedOnTime(0)
	, m_ProjectorDmdSize(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_OnTimerErrors.ResetErrors();
}

void CEasyProjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_PROJ_NONE, m_RadioBtnProjTypeNone);
	DDX_Control(pDX, IDC_RADIO_PROJ_STAR_07, m_RadioBtnProjTypeStar07);
	DDX_Control(pDX, IDC_RADIO_PROJ_STAR_07_RGB, m_RadioBtnProjTypeStar07RGB);
	DDX_Control(pDX, IDC_RADIO_PROJ_V_MODULE, m_RadioBtnProjTypeVModule);
	DDX_Text(pDX, IDC_SERIAL, m_ProjectorSerial);
	DDX_Control(pDX, IDC_SEQ_LOAD, m_BtnSequenceLoad);
	DDX_Control(pDX, IDC_SEQ_FREE, m_BtnSeqFree);
	DDX_Control(pDX, IDC_PREVIEW, m_Preview);
	DDX_Text(pDX, IDC_SEQ_IMAGES, m_SequenceLoadedImages);
	DDX_Control(pDX, IDC_ILLU_TIME, m_EditIlluminateTime);
	DDX_Control(pDX, IDC_PIC_TIME, m_EditPictureTime);
	DDX_Control(pDX, IDC_PROJ_START, m_BtnProjection);
	DDX_Control(pDX, IDC_PROJ_STOP, m_BtnProjStop);
	DDX_Control(pDX, IDC_SET_SEQ_PARAM, m_BtnSet);
	DDX_Control(pDX, IDC_SLIDER_LED_BRIGHTNESS, m_sldLedBrightness);
	DDX_Slider(pDX, IDC_SLIDER_LED_BRIGHTNESS, m_iLedBrightness);
	DDX_Control(pDX, IDC_RADIO_LED_NONE, m_RadioBtnLEDTypeNone);
	DDX_Control(pDX, IDC_RADIO_LED_RED, m_RadioBtnLEDTypeRed);
	DDX_Control(pDX, IDC_RADIO_LED_GREEN, m_RadioBtnLEDTypeGreen);
	DDX_Control(pDX, IDC_RADIO_LED_BLUE_TE, m_RadioBtnLEDTypeBlueTE);
	DDX_Control(pDX, IDC_RADIO_LED_UV, m_RadioBtnLEDTypeUV);
	DDX_Control(pDX, IDC_RADIO_LED_CBT_140_WHITE, m_RadioBtnLEDTypeCBT140White);
	DDX_Text(pDX, IDC_LED1_TEMPERATURE, LedTemperature(1));
	DDX_Text(pDX, IDC_LED2_TEMPERATURE, LedTemperature(2));
	DDX_Text(pDX, IDC_LED3_TEMPERATURE, LedTemperature(3));

	DDX_Radio(pDX, IDC_RADIO_PROJ_NONE, m_iProjType);
	DDX_Radio(pDX, IDC_RADIO_LED_NONE, m_iLEDType);
	DDX_Text(pDX, IDC_EDIT_REPORT_BITNUM, m_SequenceBitnum);
	DDX_Control(pDX, IDC_CHECK_PIC_TIME, m_CheckPictureTime);
	DDX_Control(pDX, IDC_CHECK_ILLU_TIME, m_CheckIlluminateTime);
	DDX_Control(pDX, IDC_CHECK_FLIP, m_CheckFlip);
	DDX_Text(pDX, IDC_DMD_SIZE, m_ProjectorDmdSize);
}

BEGIN_MESSAGE_MAP(CEasyProjDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_PROJ_NONE, &CEasyProjDlg::OnBnClickedRadioProjNone)
	ON_BN_CLICKED(IDC_RADIO_PROJ_STAR_07, &CEasyProjDlg::OnBnClickedRadioProjStar07)
	ON_BN_CLICKED(IDC_RADIO_PROJ_STAR_07_RGB, &CEasyProjDlg::OnBnClickedRadioProjStar07RGB)
	ON_BN_CLICKED(IDC_RADIO_PROJ_V_MODULE, &CEasyProjDlg::OnBnClickedRadioProjAlpOnly)
	ON_BN_CLICKED(IDC_SEQ_LOAD, &CEasyProjDlg::OnBnClickedSeqLoad)
	ON_BN_CLICKED(IDC_SEQ_FREE, &CEasyProjDlg::OnBnClickedSeqFree)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SET_SEQ_PARAM, &CEasyProjDlg::OnBnClickedSetSeqParam)
	ON_BN_CLICKED(IDC_PROJ_START, &CEasyProjDlg::OnBnClickedProjStart)
	ON_BN_CLICKED(IDC_PROJ_STOP, &CEasyProjDlg::OnBnClickedProjStop)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RADIO_LED_NONE, &CEasyProjDlg::OnBnClickedRadioLedNone)
	ON_BN_CLICKED(IDC_RADIO_LED_RED, &CEasyProjDlg::OnBnClickedRadioLedRed)
	ON_BN_CLICKED(IDC_RADIO_LED_GREEN, &CEasyProjDlg::OnBnClickedRadioLedGreen)
	ON_BN_CLICKED(IDC_RADIO_LED_BLUE_TE, &CEasyProjDlg::OnBnClickedRadioLedBlueTe)
	ON_BN_CLICKED(IDC_RADIO_LED_UV, &CEasyProjDlg::OnBnClickedRadioLedUv)
	ON_BN_CLICKED(IDC_RADIO_LED_CBT_140_WHITE, &CEasyProjDlg::OnBnClickedRadioLedCbt140White)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_PIC_TIME, &CEasyProjDlg::OnBnClickedCheckPicTime)
	ON_BN_CLICKED(IDC_CHECK_ILLU_TIME, &CEasyProjDlg::OnBnClickedCheckIlluTime)
	ON_BN_CLICKED(IDC_CHECK_FLIP, &CEasyProjDlg::OnBnClickedCheckFlip)
	ON_BN_CLICKED(IDHELP, &CEasyProjDlg::OnBnClickedHelp)
END_MESSAGE_MAP()


// CEasyProjDlg message handlers

BOOL CEasyProjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// -----
	m_CheckPictureTime.SetCheck( BST_CHECKED );
	UpdateElements();													// update all dialog elements from data
	// -----

	m_bOnTimerIsModal = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CEasyProjDlg::OnDestroy()
{
	// free sequence and projector before the application is finished
	m_Projector.SequenceFree();
	m_Projector.Free();

	CDialogEx::OnDestroy();
}


void CEasyProjDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEasyProjDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEasyProjDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


////////////////////////////////////////////////////////////////////////////////


// check return codes and output error messages (not all features of the function are currently used)
void CEasyProjDlg::SetProjectorReturnCodeMsg( const int retCode, const int errorType, CString exErrorMessage)
{
	CString strReport;

	m_Projector.GetErrorMessage( retCode, strReport.GetBuffer( 512), 512);
	strReport.ReleaseBuffer();
	strReport.Format(exErrorMessage + L"\nErrorcode %d : " + strReport, retCode);

	if( ALP_DEVICE_REMOVED == retCode)
		m_Projector.Free();


	if( ALP_OK != retCode)
	{
		if( FAILURE == errorType)												// a failure will cause the programm to be reset
		{
			Reset();															// reset the programm to "no projector"
			MessageBeep( MB_ICONERROR );
			MessageBox( strReport + L"\nProgram will be reset.", L"Error",  MB_ICONSTOP);
		}
		else if (WARNING == errorType)											// in case of a warning the programm will resume
		{
			MessageBeep( MB_ICONERROR );
			MessageBox( strReport + L"\nProgram will be resumed.", L"Error",  MB_ICONEXCLAMATION);
		}
		else
			throw;
	}
}

void CEasyProjDlg::Reset()
{
	m_iProjType = CProjector::PROJECTOR_TYPE_NONE;
	UpdateData( FALSE);													// update direction: data -> dialog elements
	BnClickedRadioProjectorType();	
}

// update all dialog elements from data
void CEasyProjDlg::UpdateElements(void)
{
	BOOL const bValidProjector = m_Projector.IsConnected(),
		bValidSeqence = bValidProjector && m_Projector.IsValidSequence();
	BOOL bValidStar07 = bValidProjector && CProjector::PROJECTOR_TYPE_STAR_07 == m_Projector.GetProjectorType();

	// Always enable device selection
	m_RadioBtnProjTypeNone.EnableWindow();
	m_RadioBtnProjTypeStar07.EnableWindow();
	m_RadioBtnProjTypeStar07RGB.EnableWindow();
	m_RadioBtnProjTypeVModule.EnableWindow();
	
	// Dynamically enable LED selection
	m_RadioBtnLEDTypeNone.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());
	m_RadioBtnLEDTypeRed.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());
	m_RadioBtnLEDTypeGreen.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());
	m_RadioBtnLEDTypeBlueTE.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());
	m_RadioBtnLEDTypeUV.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());
	m_RadioBtnLEDTypeCBT140White.EnableWindow(bValidStar07 && !m_Projector.Led(1).IsValid());

	m_sldLedBrightness.EnableWindow( m_Projector.Led(1).IsValid() || m_Projector.Led(2).IsValid() || m_Projector.Led(3).IsValid());

	m_BtnSequenceLoad.EnableWindow( bValidProjector && !bValidSeqence && (!bValidStar07 || m_Projector.Led(1).IsValid()));
	m_BtnSeqFree.EnableWindow( bValidSeqence);

	// Dynamically enable timing setting controls
	m_CheckPictureTime.EnableWindow( bValidSeqence );
	m_EditPictureTime.EnableWindow( bValidSeqence && m_CheckPictureTime.GetCheck() );
	m_CheckIlluminateTime.EnableWindow( bValidSeqence );
	m_EditIlluminateTime.EnableWindow( bValidSeqence && m_CheckIlluminateTime.GetCheck() );

	m_BtnProjection.EnableWindow( bValidSeqence);
	m_BtnProjStop.EnableWindow( bValidSeqence);
	m_BtnSet.EnableWindow( bValidSeqence);
	m_CheckFlip.EnableWindow( bValidProjector );
	m_CheckFlip.SetCheck( m_Projector.GetFlip() );

	UpdateData( FALSE);													// update direction: data -> dialog elements
}

// get projector properties and update dialog elements
const int CEasyProjDlg::GetProjectorProperties(CString *errorMessage)
{
	CProjector::CDevProperties prop;
	int ret = m_Projector.GetDevProperties( prop, errorMessage);
	if( ALP_OK == ret)
	{
		m_ProjectorSerial.Format( _T("%d"), prop.SerialNumber);
		m_ProjectorDmdSize.Format( _T("%d x %d"), m_Projector.GetWidth(), m_Projector.GetHeight() );
	}
	return ret;
}


const int CEasyProjDlg::SetRoundRobin(CString *errorMessage)
{
	tAlpDynSynchOutGate GateConfig;
	ZeroMemory( GateConfig.Gate, sizeof GateConfig.Gate );			// all Gates are set to zero

	GateConfig.Period   = 3;
	GateConfig.Polarity = 1;

	GateConfig.Gate[0] = 1;

	int ret = m_Projector.SetSynchGate( 1, GateConfig, errorMessage);

	GateConfig.Gate[0] = 0;
	GateConfig.Gate[1] = 1;

	if(ALP_OK == ret)
		ret = m_Projector.SetSynchGate( 2, GateConfig, errorMessage);

	GateConfig.Gate[1] = 0;
	GateConfig.Gate[2] = 1;

	if(ALP_OK == ret)
		ret = m_Projector.SetSynchGate( 3, GateConfig, errorMessage);

	m_LedEnableMode = GATE_ROUND_ROBIN3;

	return ret;
}

const int CEasyProjDlg::SetConstHigh(CString *errorMessage)
{
	int ret;
	tAlpDynSynchOutGate GateConfig;
	ZeroMemory( GateConfig.Gate, sizeof GateConfig.Gate );		// all Gates are set to zero

	GateConfig.Period   = 1;
	GateConfig.Polarity = 0;

	// the Led of STAR-07 is connected to SynchGate 3
	ret = m_Projector.SetSynchGate( 3, GateConfig, errorMessage);

	m_LedEnableMode = GATE_HIGH;

	return ret;
}


void CEasyProjDlg::BnClickedRadioProjectorType()
{
	CWaitCursor WaitCursor; 

	UpdateData( TRUE);													// update direction: dialog elements -> data	
	if(m_Projector.IsConnected())
		FreeProjector();
	m_Projector.SetProjectorType(m_iProjType);
	AllocateProjector();

	if (CProjector::PROJECTOR_TYPE_STAR_07 != m_iProjType)
		m_iLEDType = 0;

	UpdateElements();													// update all dialog elements from data
}


// Sub-function
void CEasyProjDlg::BnClickedRadioLEDType()
{
	CString exErrorMessage;

	// Note: The Radio Button Index must match CProjector::GetLedTypeByIndex!

	UpdateData( TRUE);													// update direction: dialog elements -> data
	int ret = SelectLEDType( m_iLEDType);
	if( ALP_OK != ret)
		m_iLEDType = 0;
	SetProjectorReturnCodeMsg(ret, WARNING, exErrorMessage);

	m_iLedBrightness = 0;
	SetLEDBrightness( m_iLedBrightness);
	UpdateElements();													// update all dialog elements from data
}


// select the LED type
const int CEasyProjDlg::SelectLEDType( const int iLedTypeIndex, CString *errorMessage)
{
	CString exErrorMessage;
	int ret;

	if(m_Projector.Led(1).IsValid())
		{
			ret = m_Projector.Led(1).Free(&exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
		}

	if( iLedTypeIndex == CProjector::LEDTYPE_0_NONE) return ALP_OK;

	ret = m_Projector.Led(1).Alloc( m_Projector.GetLedTypeByIndex( iLedTypeIndex), 0, errorMessage);

	return ret;
}


// change the LED brightness
void CEasyProjDlg::SetLEDBrightness( const int iBrightness)
{
	long ret = ALP_OK;
	CString exErrorMessage;

		if(m_Projector.Led(1).IsValid())
		{
			ret = m_Projector.Led(1).SetBrightness( iBrightness, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
		}
		if(m_Projector.Led(2).IsValid())
		{
			ret = m_Projector.Led(2).SetBrightness( iBrightness, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
		}
		if(m_Projector.Led(3).IsValid())
		{
			ret = m_Projector.Led(3).SetBrightness( iBrightness, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
		}
}


// free sequence and reset dialog elements
const int CEasyProjDlg::FreeSequence(CString *errorMessage)
{
	int ret = ALP_OK;
	if( m_Projector.IsValidSequence())
	{
		ret = m_Projector.SequenceFree(errorMessage);
	}
	m_SequenceLoadedImages = CString( L" ");
	SetDlgItemInt( IDC_ILLU_TIME, 0 );
	SetDlgItemInt( IDC_PIC_TIME, 0 );
	m_SequenceBitnum.Empty();

	return ret;
}


// get sequence properties
const int CEasyProjDlg::GetSequenceProperties(CString *errorMessage)
{
	CProjector::CTimingEx timing;
	int ret = m_Projector.GetSeqProperties( timing, errorMessage);
	if( ALP_OK == ret)
	{
		SetDlgItemInt( IDC_ILLU_TIME, timing.IlluminateTime );
		SetDlgItemInt( IDC_PIC_TIME, timing.PictureTime );
		
		// Certain timing values are required later on for LED temperature monitoring.
		m_PictureTime = timing.PictureTime;
		m_LedOnTime = timing.SynchPulseWidth;

		m_SequenceBitnum.Format( _T("%d"), timing.BitNum );
		if (timing.Uninterrupted) m_SequenceBitnum += _T(" uninterrupted");
	}
	return ret;
}


// change sequence properties
const int CEasyProjDlg::SetSequenceProperties(CString *errorMessage)
{
	UpdateData( TRUE);													// update direction: dialog elements -> data

	CProjector::CTimingEx timing;

	if (m_CheckIlluminateTime.GetCheck())
		timing.IlluminateTime = GetDlgItemInt( IDC_ILLU_TIME );
	else
		timing.IlluminateTime = ALP_DEFAULT;

	if (m_CheckPictureTime.GetCheck())
		timing.PictureTime = GetDlgItemInt( IDC_PIC_TIME );
	else
		timing.PictureTime = ALP_DEFAULT;

	if (CProjector::PROJECTOR_TYPE_STAR_07_RGB == m_Projector.GetProjectorType())
		timing.SynchDelay = 20;	// activate each LED some micro seconds before IlluminateTime starts

	// m_SequenceBitnum is read-only

	return m_Projector.SelectMaxBitnum( timing, errorMessage);
}


// load images and add it to a sequence
const int CEasyProjDlg::LoadSequence(CString *errorMessage)
{
	const LPCTSTR filter =
		_T("All Image Files|*.tif;*.tiff;*.png;*.gif;*.bmp;*.jpg;*.jpeg|")
		_T("TIF Files for Sequence (*.tif)|*.tif|")
		_T("TIFF Files for Sequence (*.tiff)|*.tiff|");
	CFileDialog dlg( TRUE, NULL, 0, OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST|OFN_ENABLESIZING, filter);

	wchar_t buffer[1<<16];
	*buffer = '\0';

	dlg.m_pOFN->lpstrFile	= buffer;
	dlg.m_pOFN->nMaxFile	= sizeof(buffer)/sizeof(*buffer);

	int ret = ALP_OK;
	int nImages = 0;

	if(IDOK == dlg.DoModal())											// show file open dialog
	{
		CWaitCursor WaitCursor; 

		// determine the sequence length
		for(POSITION pos = dlg.GetStartPosition(); NULL!=pos; )
		{
			CString fileNameLOAD = dlg.GetNextPathName(pos);
			nImages ++;
		}


		const int BitPlanes = 8;									// use all bitplanes for projection
		ret = m_Projector.SequenceAlloc( BitPlanes, nImages*m_ColourComponents, errorMessage);		// create a sequence
		if( ALP_OK != ret)
			return ret;

		// read images from file, convert it and put it to the projector
		Gdiplus::Status status;
		int k = 0;	
		for(POSITION pos = dlg.GetStartPosition(); NULL!=pos; )
		{
			CString fileNameLOAD = dlg.GetNextPathName(pos);

			Gdiplus::Bitmap BmpLoaded( fileNameLOAD);			// read the image from file

			{	// Show progress and preview thumbnail
				m_SequenceLoadedImages.Format( _T("%d"), k+1);	// update dialog element with current image counter

				// create a bitmap with preview dimensions
				const int thumbWidth = 176;
				const int thumbHeight = 132;
				std::auto_ptr<Gdiplus::Bitmap> pThumbBmp(new Gdiplus::Bitmap( thumbWidth, thumbHeight, PixelFormat24bppRGB));
				Gdiplus::Graphics thumbGraphics(pThumbBmp.get());	// connect the drawing area with preview

				// draw BmpLoaded in the drawing area of the preview
				status = thumbGraphics.DrawImage( &BmpLoaded, 0, 0, thumbWidth, thumbHeight);	
				HBITMAP hBmp = 0;
				status = pThumbBmp->GetHBITMAP( Gdiplus::Color(0,0,0), &hBmp);	// get bitmap handle
				DeleteObject(m_Preview.SetBitmap( hBmp ));	// draw the preview, dereference previous bitmap
				DeleteObject(hBmp);
			}


			// create a new bitmap with projector dimensions
			std::auto_ptr<Gdiplus::Bitmap> pProjBmp(new Gdiplus::Bitmap( m_Projector.GetWidth(), m_Projector.GetHeight(), PixelFormat24bppRGB));
			Gdiplus::Graphics projGraphics( pProjBmp.get() );

			for(int splitPicNum=0; splitPicNum<m_ColourComponents; splitPicNum++) // for RGB projection, a source-picture is splited in to three target-pictures 
			{
				// create a 8bpp-sequence-image as bitmap with projector dimensions
				std::auto_ptr<Gdiplus::Bitmap> pSeqImageBmp(new Gdiplus::Bitmap( m_Projector.GetWidth(), m_Projector.GetHeight(), PixelFormat8bppIndexed));
	
				if( BmpLoaded.GetWidth() == pProjBmp->GetWidth()
					&&	BmpLoaded.GetHeight() == pProjBmp->GetHeight())
				{
					// resize not necessary -> clone the bitmap
					pProjBmp.reset(BmpLoaded.Clone( 0, 0, m_Projector.GetWidth(), m_Projector.GetHeight(), PixelFormat24bppRGB));
				}
				else
				{
					// draw bitmap into the drawing area of the sequence image, resize
					status = projGraphics.DrawImage( &BmpLoaded, 0, 0, m_Projector.GetWidth(), m_Projector.GetHeight());
				}

				if( pProjBmp->GetWidth() != pSeqImageBmp->GetWidth()
					||	pProjBmp->GetHeight() != pSeqImageBmp->GetHeight())
					break;

				// lock RGB image for read access
				Gdiplus::Rect lockRect( 0, 0, pProjBmp->GetWidth(), pProjBmp->GetHeight());	
				Gdiplus::BitmapData bitmapDataProj;		// image data
				pProjBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapDataProj);

				// lock 8bpp sequence image for write access
				Gdiplus::BitmapData bitmapData8bpp;		// image data
				pSeqImageBmp->LockBits( &lockRect, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, &bitmapData8bpp);

				BYTE *pImageDataProj = static_cast<BYTE*>(bitmapDataProj.Scan0);	// pointer to the first pixel of the first line (RGB image: source)
				BYTE *pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// pointer to the first pixel of the first line (8bpp image: target)

				// transform pixel wise: RGB -> 8bpp
				if(3 == m_ColourComponents)						// RGB picture
				{
					// Byte-order of color components: B,G,R
					// see also GdiPlusPixelFormats: BLUE_SHIFT=0, GREEN_SHIFT=8, RED_SHIFT=16
					int const nColorIndex = 2-splitPicNum;
					for( size_t y=0; y < pProjBmp->GetHeight(); y++)
					{
						for( size_t x=0; x < pProjBmp->GetWidth(); x++)
						{
							pImageData8bpp[x] =	( pImageDataProj[ x*3 + nColorIndex]);
						}
						pImageDataProj += bitmapDataProj.Stride;				// set pointer to the first pixel of the next line
						pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
					}
				} else
				{	// monochrome
					for( size_t y=0; y < pProjBmp->GetHeight(); y++)
					{
						for( size_t x=0; x < pProjBmp->GetWidth(); x++)
						{
							pImageData8bpp[x] =	( pImageDataProj[ x*3]		// B
												+ pImageDataProj[ x*3+1]	// G
												+ pImageDataProj[ x*3+2]	// R
												) / 3;
						}
						pImageDataProj += bitmapDataProj.Stride;				// set pointer to the first pixel of the next line
						pImageData8bpp += bitmapData8bpp.Stride;				// set pointer to the first pixel of the next line
					}
				}

				pImageData8bpp = static_cast<BYTE*>(bitmapData8bpp.Scan0);	// set pointer back to the first pixel of the first line (RGB image: source)
				ret = m_Projector.AddImage( pImageData8bpp, pSeqImageBmp->GetWidth(), pSeqImageBmp->GetHeight(), errorMessage);		// upload image to the projector an add to sequence

				status = pProjBmp->UnlockBits(&bitmapDataProj);	// lock end
				status = pSeqImageBmp->UnlockBits(&bitmapData8bpp);	// lock end

				if( ALP_OK != ret)
					return ret;
			}

			k++;
		}
	}
	return ret;
}

CString &CEasyProjDlg::LedTemperature(int nIndex)
{
	switch (nIndex)
	{
	default: throw;
	case 1: return m_Led1Temperature;
	case 2: return m_Led2Temperature;
	case 3: return m_Led3Temperature;
	}
}

////////////////////////////////////////////////////////////////////////////////


void CEasyProjDlg::OnBnClickedRadioProjNone()
{
	BnClickedRadioProjectorType();
}

void CEasyProjDlg::OnBnClickedRadioProjStar07()
{
	BnClickedRadioProjectorType();
}

void CEasyProjDlg::OnBnClickedRadioProjStar07RGB()
{
	BnClickedRadioProjectorType();
}

void CEasyProjDlg::OnBnClickedRadioProjAlpOnly()
{
	BnClickedRadioProjectorType();
}

void CEasyProjDlg::AllocateProjector()
{
	if(CProjector::PROJECTOR_TYPE_NONE != m_Projector.GetProjectorType())
	{
		CString exErrorMessage;
		
		int ret = m_Projector.Alloc(&exErrorMessage);
		
		SetProjectorReturnCodeMsg( ret, FAILURE, exErrorMessage);
		if(ALP_OK != ret)
			return;

		m_LedEnableMode = GATE_LOW;

		ret = GetProjectorProperties(&exErrorMessage);
		
		SetProjectorReturnCodeMsg( ret, FAILURE, exErrorMessage);
		if(ALP_OK != ret)
			return;

		switch(m_Projector.GetProjectorType())
		{
		case CProjector::PROJECTOR_TYPE_ALP_ONLY:
			m_ColourComponents = 1;
			break;
		case CProjector::PROJECTOR_TYPE_STAR_07:
			ret = SetConstHigh(&exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
			m_ColourComponents = 1;
			break;
		case CProjector::PROJECTOR_TYPE_STAR_07_RGB:
			m_ColourComponents = 3;
			ret = SetRoundRobin(&exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

			ret = m_Projector.Led(1).Alloc(ALP_HLD_PT120_RED, 0, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, L"Led 1 : " + exErrorMessage);
			ret = m_Projector.Led(2).Alloc(ALP_HLD_PT120_GREEN, 1, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, L"Led 2 : " + exErrorMessage);
			ret = m_Projector.Led(3).Alloc(ALP_HLD_PT120TE_BLUE, 2, &exErrorMessage);
			SetProjectorReturnCodeMsg( ret, WARNING, L"Led 3 : " + exErrorMessage);
			break;
		default:
			throw;
		}	
	}
	UpdateElements();													// update all dialog elements from data

	// LED Monitor Timer starts now, before LEDs are actually switched on.
	SetTimer(MONITOR, MONITOR_PERIOD, NULL);
	OnTimer(MONITOR);
}


void CEasyProjDlg::FreeProjector()
{
	CString exErrorMessage;
	m_iLedBrightness = 0;
	SetLEDBrightness( m_iLedBrightness);
	int ret = m_Projector.ProjStop(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	
	ret = FreeSequence(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

	// free projector and reset dialog elements
	ret = m_Projector.Free(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

	m_ProjectorSerial = CString( L" ");									// delete the serial number
	m_ProjectorDmdSize = CString( L" ");
	m_iLEDType = 0;
	m_iLedBrightness = 0;
	m_Led1Temperature = "";
	m_Led2Temperature = "";
	m_Led3Temperature = "";
	m_LedEnableMode = GATE_LOW;
	m_OnTimerErrors.ResetErrors();
	KillTimer(MONITOR);
	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnBnClickedRadioLedNone()
{
	BnClickedRadioLEDType();
}
void CEasyProjDlg::OnBnClickedRadioLedRed()
{
	BnClickedRadioLEDType();
}
void CEasyProjDlg::OnBnClickedRadioLedGreen()
{
	BnClickedRadioLEDType();
}
void CEasyProjDlg::OnBnClickedRadioLedBlueTe()
{
	BnClickedRadioLEDType();
}
void CEasyProjDlg::OnBnClickedRadioLedUv()
{
	BnClickedRadioLEDType();
}
void CEasyProjDlg::OnBnClickedRadioLedCbt140White()
{
	BnClickedRadioLEDType();
}


void CEasyProjDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
	// all other SB codes cause a subsequent SB_ENDSCROLL
	case SB_ENDSCROLL:   // End scroll.
			UpdateData( TRUE);											// update direction: dialog elements -> data
			SetLEDBrightness( m_iLedBrightness);
			break;
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CEasyProjDlg::OnBnClickedSeqLoad()
{
	CString exErrorMessage;
	
	int ret = LoadSequence(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	
	ret = GetSequenceProperties(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	
	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnBnClickedSeqFree()
{
	CString exErrorMessage;
	
	int ret = FreeSequence(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnBnClickedSetSeqParam()
{
	CString exErrorMessage;	
	
	int ret = SetSequenceProperties(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

	// In case of a running projection the changes gets effective even after Stop/Start of the projection.
	if( ALP_OK == ret
	&&	m_Projector.IsProjection())
	{
		ret = m_Projector.ProjStop(&exErrorMessage);
		SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);

		ret = m_Projector.ProjStartContinuous(&exErrorMessage);
		SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	}

	// After successful timing setup, report effective settings. These values could
	// differ from input in case of inconsistent parameters or ALP_DEFAULT.
	// On failure the user interface parameters shall be adjustable individually, so they are not overwritten.
	if (ALP_OK==ret)
		ret = GetSequenceProperties(&exErrorMessage);
	
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnBnClickedProjStart()
{
	CString exErrorMessage;	
	
	int ret = m_Projector.ProjStartContinuous(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	
	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnBnClickedProjStop()
{
	CString exErrorMessage;	

	int ret = m_Projector.ProjStop(&exErrorMessage);
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
	
	UpdateElements();													// update all dialog elements from data
}


void CEasyProjDlg::OnOK()
{
	OnBnClickedSetSeqParam();
}

void CEasyProjDlg::OnCancel()
{
	if (IDYES == MessageBox( _T("Quit?"), NULL, MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1 ))
		CDialogEx::OnCancel();
}


void CEasyProjDlg::OnBnClickedHelp()
{
	WinHelp( 0 );
}

void CEasyProjDlg::WinHelp(DWORD dwData, UINT nCmd)
{
	UNREFERENCED_PARAMETER( (dwData, nCmd) );

	MessageBox(
		_T("An introduction to EasyProj can be found in the ALP Quick Start Guide.\r\n")
		_T("1. Select Device Type: this intiializes the V-Module or STAR-07 projector\r\n")
		_T("2. STAR-07: Select the correct LED type.\r\n")
		_T("   The wrong type could cause overload and damage.\r\n")
		_T("3. Load pictures in order to create an ALP sequence.\r\n")
		_T("   STAR-07 RGB uses colour components, other devices convert to monochrome.\r\n")
		_T("4. Start projection.\r\n")
		_T("5. Sequence Timing automatically corrects inconsistent parameters.\r\n")
		_T("Please read the source code and the ALP API description for details."),
		_T("ALP EasyProj - ViALUX GmbH")
		);
}


void CEasyProjDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString exErrorMessage;
	CString strReport;
	long Temp, TempLimit;

	switch (nIDEvent)
	{
	case MONITOR:
		
		for(int i=1; i<=3; i++)
		{
			if(m_Projector.Led(i).IsValid())
			{
				OnTimerInquiryHandler(i, Temp, TempLimit);
				// Output estimation of LED Junction Temperature:
				LedTemperature(i).Format( _T("%3.1f"), double(Temp)/256);
				
				// check for temperature exceedance 
				if (Temp > TempLimit*256)
				{
					m_Projector.Led(i).OverTempAlertsAdd1();
					LedTemperature(i) += _T(" !");
				} else
				{
					m_Projector.Led(i).RstOverTempAlerts();
				}
				// temperatur exceedance three times in a row  --> emergency stop 
				if (m_Projector.Led(i).GetOverTempAlerts() == 3)
				{
					m_iLedBrightness = 0;
					SetLEDBrightness( m_iLedBrightness);
					UpdateElements();													// update all dialog elements from data

					BOOL bHaveFocus;
					strReport.Format( _T("Led %d is overheating.\nEmergency stop."), i);
					if (OnTimerGetFocus(bHaveFocus))
						MessageBox(strReport, L"Warning", MB_ICONEXCLAMATION);
					OnTimerKillFocus( bHaveFocus );
					m_Projector.Led(i).RstOverTempAlerts();
				}
			}
			else
			{
				LedTemperature(i).Format(_T(""));
			}
		}
		UpdateData(FALSE);

		// reset time-out watchdog: shut down LEDs 0.5s - 1.5s after the next scheduled monitor event.
		// (silently ignore return value -> older ALP API DLLs do not implement this feature)
		m_Projector.RetriggerSynchOutWatchdog( (MONITOR_PERIOD+1500)/1000 );

		break;

	default:
		CDialog::OnTimer(nIDEvent);
	};
}

	BOOL m_bOnTimerIsModal;
BOOL CEasyProjDlg::OnTimerGetFocus( OUT BOOL &bHaveFocus )
{
	if (m_bOnTimerIsModal)
		bHaveFocus = FALSE;
	else
	{
		// Now I have the focus, and can run a modal message box.
		bHaveFocus =
		m_bOnTimerIsModal = TRUE;
	}
	return bHaveFocus;
}
void CEasyProjDlg::OnTimerKillFocus( IN OUT BOOL &bHaveFocus)
{
	if (bHaveFocus)
		// Reset class member only if _I_ have had the focus!
		m_bOnTimerIsModal = FALSE;
	bHaveFocus = FALSE;
}

void CEasyProjDlg::OnTimerInquiryHandler(int Indx, long &JunctionTemp, long &JunctionTempLimit)
{
	CString exErrorMessage;	
	
	// Consider actual Pulse Timing in Micro Seconds
	long OnTime = 0, PeriodTime = 0;
	switch (m_LedEnableMode)
	{
	default:
	case GATE_LOW :
		OnTime = 0; PeriodTime = 100;	// Off
		break;
	case GATE_ROUND_ROBIN3 :
		OnTime = m_Projector.IsProjection() ? m_LedOnTime : 0;
		PeriodTime = 3*m_PictureTime;
		break;
	case GATE_HIGH :
		OnTime = 100; PeriodTime = 100;	// Continuous Wave
		break;
	};
	
	long ret = m_Projector.Led(Indx).GetJunctionTemperature(JunctionTemp, &exErrorMessage, OnTime, PeriodTime);
	JunctionTempLimit = m_Projector.Led(Indx).GetJunctionTempLimit();

	if(!m_OnTimerErrors.m_LedTempError[Indx])
	{
		m_OnTimerErrors.m_LedTempError[Indx] = ret;
		exErrorMessage.Format(L"Led  %d : " + exErrorMessage, Indx);

		BOOL bHaveFocus;
		if (OnTimerGetFocus(bHaveFocus))
			SetProjectorReturnCodeMsg(ret, WARNING, exErrorMessage);
		OnTimerKillFocus(bHaveFocus);
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////

void CEasyProjDlg::COnTimerErrors::ResetErrors()
{
	for(int i=0; i<3; i++)
	{
		m_LedTempError[i] = ALP_OK;
	}
}


void CEasyProjDlg::OnBnClickedCheckPicTime()
{
	UpdateElements();
}

void CEasyProjDlg::OnBnClickedCheckIlluTime()
{
	UpdateElements();
}


void CEasyProjDlg::OnBnClickedCheckFlip()
{
	CString exErrorMessage;
	long ret;
	ret = m_Projector.SetFlip( m_CheckFlip.GetCheck(), exErrorMessage );
	SetProjectorReturnCodeMsg( ret, WARNING, exErrorMessage);
}
