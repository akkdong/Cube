
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

	double mCenterPos[2]; // drawing center lat/lon
	double mBoundary[4]; // left(W), top(N), right(E), bottom(S)

	double mTotalDist;
	double mOptimizedDist;

	double mZoomRatio = 1.0;
	CPoint mDrawOffset;

	std::vector<double> mTheta;

// Operations
public:
	void RecalcLayout(int cx, int cy);

	void DrawCircle(CDC* pDC, int cx, int cy, double lat, double lon, double radius);
	void DrawLine(CDC* pDC, int cx, int cy, double lat1, double lon1, double lat2, double lon2);

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();

	DECLARE_MESSAGE_MAP()
};

