#if !defined(AFX_MAINDLG_H__41C02363_A302_481B_AC9E_AAAB119712F4__INCLUDED_)
#define AFX_MAINDLG_H__41C02363_A302_481B_AC9E_AAAB119712F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CmainDlg dialog

class CmainDlg : public CDialog
{
public://手动添加-------------
	int MySetPixelFormat(HDC hdc);
	HDC hdc ;
	HGLRC hglrc;
	void run();	
	void draw();
	void addText(const char* text, float x, float y,bool bkDim=true);//在屏幕上追加文字（默认值写在声明处）
	void unload();//卸载
	void reSetCam(float xmin,float xmax,float zmin,float zmax);//根据场景范围重置相机
	void updateMenuItemEnableState();//更新菜单项的禁用状态
// Construction
public:
	CmainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CmainDlg)
	enum { IDD = IDD_mainDlg };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CmainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CmainDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void Onopen();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void Onrun();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void Onclose();
	afx_msg void Onabout();
	afx_msg void Onsave();
	afx_msg void Onloadmodel();
	afx_msg void OnClose();
	afx_msg void OnrunSingle();
	afx_msg void Onseting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__41C02363_A302_481B_AC9E_AAAB119712F4__INCLUDED_)
