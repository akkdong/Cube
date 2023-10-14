
// ChildView.h : interface of the CChildView class
//

#pragma once

#include "Route.h"

#define GEO_W		(0)
#define GEO_N		(1)
#define GEO_E		(2)
#define GEO_S		(3)

#define GEO_LAT		(0)
#define GEO_LON		(1)


// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	XcTask mTask;

	XcBasePoint mCenterPos; // drawing center lat/lon
	double mBoundary[4]; // left(W), top(N), right(E), bottom(S)

	double mZoomRatio = 1.0;
	CPoint mDrawOffset;

	std::shared_ptr<XcBasePoint> mPilotPosPtr;
	bool mPilotInCylinder;
	size_t mStartTurnPoint;

	//
	CButton m_wndOpen;
	CButton m_wndZoomIn;
	CButton m_wndZoomOut;
	CButton m_wndNext;
	CButton m_wndPrev;

#if 0
	double mTotalDist;
	double mOptimizedDist;

	std::vector<double> mTheta;
#endif

// Operations
public:
	void RecalcLayout(int cx, int cy);

	void DrawCircle(CDC* pDC, int cx, int cy, double lat, double lon, double radius);
	void DrawLine(CDC* pDC, int cx, int cy, double lat1, double lon1, double lat2, double lon2);
	void DrawPilot(CDC* pDC, int cx, int cy, double lat, double lon);

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG *pMsg);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileOpen();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnMoveNext();
	afx_msg void OnMovePrev();

	DECLARE_MESSAGE_MAP()
};

