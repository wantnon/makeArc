#if !defined(AFX_SETINGDLG_H__3E133BB7_4C74_45B6_83F0_BA576A1DBEFA__INCLUDED_)
#define AFX_SETINGDLG_H__3E133BB7_4C74_45B6_83F0_BA576A1DBEFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// setingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CsetingDlg dialog

class CsetingDlg : public CDialog
{
// Construction
public:
	CsetingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CsetingDlg)
	enum { IDD = IDD_seting };
	int		m_thOfRad123;
	BOOL	m_check1;
	int		m_thOfRad45;
	int		m_thOfRad678;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsetingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CsetingDlg)
	afx_msg void OnButton1();
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETINGDLG_H__3E133BB7_4C74_45B6_83F0_BA576A1DBEFA__INCLUDED_)
