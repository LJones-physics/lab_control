//
// EasyProjDlg.h : header file
//

#pragma once

#include "Projector.h"	// include the projector class
#include "afxwin.h"

#include "PlusGdi.h"	// GDI+
#include "afxcmn.h"

// CEasyProjDlg dialog
class CEasyProjDlg : public CDialogEx
{
private:
	CProjector	m_Projector;	// instance of the projector
	long m_ColourComponents;	// for STAR-07 RGB, each picture is decomposed to 3 colour components; otherwise display is monochrome (1 colour component)

// Construction
public:
	CEasyProjDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EASYPROJ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();

	// Control: Radio button for the projectortype NONE
	CButton m_RadioBtnProjTypeNone;
	// Control: Radio button for the projectortype STAR 07
	CButton m_RadioBtnProjTypeStar07;
	// Control: Radio button for the projectortype STAR 07 RGB
	CButton m_RadioBtnProjTypeStar07RGB;
	// Control: Radio button for the projectortype V-Module
	CButton m_RadioBtnProjTypeVModule;
	// int member (Radio button) for the projectortype
	int m_iProjType;
	// CString member of the dialog element which shows the projector serial number
	CString m_ProjectorSerial;
	// CString member of the dialog element which shows the projector DMD size
	CString m_ProjectorDmdSize;
	// -----
	// Control: Radio button for the LED type NONE
	CButton m_RadioBtnLEDTypeNone;
	// Control: Radio button for the LED type RED
	CButton m_RadioBtnLEDTypeRed;
	// Control: Radio button for the LED type GREEN
	CButton m_RadioBtnLEDTypeGreen;
	// Control: Radio button for the LED type BLUE
	CButton m_RadioBtnLEDTypeBlueTE;
	// Control: Radio button for the LED type UV
	CButton m_RadioBtnLEDTypeUV;
	// Control: Radio button for the LED type WHITE
	CButton m_RadioBtnLEDTypeCBT140White;
	// -----
	// int member (Radio button) for the LED type
	int m_iLEDType;
	// Control: Slider for the LED brightness
	CSliderCtrl m_sldLedBrightness;
	// int member (Slider) for the LED brightness
	int m_iLedBrightness;
	// -----
	// Control: Button "Load images"
	CButton m_BtnSequenceLoad;
	// CString member of the dialog element which shows the number of loaded images
	CString m_SequenceLoadedImages;
	// Control: Button "Free sequence"
	CButton m_BtnSeqFree;
	// Preview of the loaded images
	CStatic m_Preview;

	// Control: Flip Check Box (Mirror image Upside-Down)
	CButton m_CheckFlip;

	// Control of Illuminate Time Check Box
	CButton m_CheckIlluminateTime;
	// Control of Picture Time Check Box
	CButton m_CheckPictureTime;

	// Control: dialog element for the illumination time of the sequence
	CEdit m_EditIlluminateTime;
	// Control: dialog element for the picture time of the sequence
	CEdit m_EditPictureTime;

	// Control: Button "Start projection
	CButton m_BtnProjection;
	// Control: Button "Stop projection"
	CButton m_BtnProjStop;
	// Control: Button "Set sequence parameter"
	CButton m_BtnSet;

	// LED Enable Timing
	enum {
		GATE_LOW,
		GATE_ROUND_ROBIN3,
		GATE_HIGH
	} m_LedEnableMode;
	long m_PictureTime, m_LedOnTime;


private:
	// Suppress display of two modal message boxes from the OnTimer message handler:
	BOOL m_bOnTimerIsModal;
	BOOL OnTimerGetFocus( OUT BOOL &bHaveFocus );
	void OnTimerKillFocus( IN OUT BOOL &bHaveFocus);

public:
	// check return codes and output error messages
	void SetProjectorReturnCodeMsg( const int retCode, const int errorType = FAILURE, CString exErrorMessage = L"");
	// reset the programm to "no projector"
	void Reset(void);
	// update all dialog elements from data
	void UpdateElements(void);
	// -----
	// allocate projector
	void AllocateProjector(void);
	// free projector and reset dialog elements
	void FreeProjector(void);
	// get projector properties and update dialog elements
	const int GetProjectorProperties(CString *errorMessage = NULL);
	// set gates for LED control to "Round Robin"
	const int SetRoundRobin(CString *errorMessage = NULL);
	// set gate for LED control to "Const. high"
	const int SetConstHigh(CString *errorMessage = NULL);
	// Sub-functions
	void BnClickedRadioLEDType();
	void BnClickedRadioProjectorType();
	// select the LED type
	const int SelectLEDType( const int iLedTypeIndex, CString *errorMessage = NULL);
	// change the LED brightness
	void SetLEDBrightness( const int iBrightness);
	// -----
	// free sequence and reset dialog elements
	const int FreeSequence(CString *errorMessage = NULL);
	// get sequence properties
	const int GetSequenceProperties(CString *errorMessage = NULL);
	// change sequence properties
	const int SetSequenceProperties(CString *errorMessage = NULL);
	// load images and add it to a sequence
	const int LoadSequence(CString *errorMessage = NULL);
	// -----
	// function for Led-temperature-inquiry
	CString &LedTemperature(int Index);
	// CStrings for Led-temperature inquiries
	CString m_Led1Temperature;
	CString m_Led2Temperature;
	CString m_Led3Temperature;
	//
	afx_msg void OnBnClickedRadioProjNone();
	afx_msg void OnBnClickedRadioProjStar07();
	afx_msg void OnBnClickedRadioProjStar07RGB();
	afx_msg void OnBnClickedRadioProjAlpOnly();
	afx_msg void OnBnClickedSeqLoad();
	afx_msg void OnBnClickedSeqFree();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSetSeqParam();
	afx_msg void OnBnClickedProjStart();
	afx_msg void OnBnClickedProjStop();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioLedNone();
	afx_msg void OnBnClickedRadioLedRed();
	afx_msg void OnBnClickedRadioLedGreen();
	afx_msg void OnBnClickedRadioLedBlueTe();
	afx_msg void OnBnClickedRadioLedUv();
	afx_msg void OnBnClickedRadioLedCbt140White();
	afx_msg void OnBnClickedCheckPicTime();
	afx_msg void OnBnClickedCheckIlluTime();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// report ALP_BITNUM and ALP_BIN_MODE
	CString m_SequenceBitnum;
	afx_msg void OnBnClickedHelp();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

	enum 
	{
		FAILURE = 0,
		WARNING = 1,
	}	ErrorTypeID;

	// Members for Led-temperature-inquiry
		enum tTimer {
		MONITOR = 1,
		MONITOR_PERIOD = 1000,	// [milli seconds]
	};
	
	class COnTimerErrors
	{
	public:
	long m_LedTempError[3];
	
	void ResetErrors();
	};

	COnTimerErrors m_OnTimerErrors;

	// Sub-function
	void OnTimerInquiryHandler(int Indx, long &JunctionTemp, long &JunctionTempLimit);
	afx_msg void OnBnClickedCheckFlip();
};
