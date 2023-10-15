
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

	class TaskManager
	{
	public:
		TaskManager(XcTask &_task, Math::real _lat, Math::real _lon)
			: task(_task), pilotPos(_lat, _lon), nextTarget(1), insideCylinder(false)
		{
			auto ptr = task.getTurnPoint(nextTarget);
			if (ptr)
				insideCylinder = ptr->inside(_lat, _lon);
		}

		XcBasePoint pilotPos;
		size_t nextTarget; // [1 ... task-count - 1]
		bool insideCylinder; // whether current pilot position is inside or not

		XcTask &task;
	};

// Attributes
public:
	XcTask mTask;

	XcBasePoint mCenterPos; // drawing center lat/lon
	Math::real mBoundary[4]; // left(W), top(N), right(E), bottom(S)

	Math::real mZoomRatio = 1.0;
	CPoint mDrawOffset;

	std::shared_ptr<XcBasePoint> mPilotPosPtr;
	bool mPilotInCylinder;
	size_t mStartTurnPoint;

	std::shared_ptr<TaskManager> mTaskManPtr;


	//
	CButton m_wndOpen;
	CButton m_wndZoomIn;
	CButton m_wndZoomOut;
	CButton m_wndNext;
	CButton m_wndPrev;

#if 0
	Math::real mTotalDist;
	Math::real mOptimizedDist;

	std::vector<Math::real> mTheta;
#endif

// Operations
public:
	void RecalcLayout(int cx, int cy);

	void DrawCircle(CDC* pDC, int cx, int cy, Math::real lat, Math::real lon, Math::real radius);
	void DrawLine(CDC* pDC, int cx, int cy, Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2);
	void DrawPilot(CDC* pDC, int cx, int cy, Math::real lat, Math::real lon);

	void Optimize(size_t startPoint, Math::real lat, Math::real lon);

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileOpen();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnMoveNext();
	afx_msg void OnMovePrev();

	DECLARE_MESSAGE_MAP()
};

