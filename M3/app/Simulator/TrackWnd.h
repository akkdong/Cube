#pragma once

//

#define MAX_POSITION		50

struct Position
{
	double lat;
	double lon;

	double track;
};

struct Point
{
	double x;
	double y;
};

// CTrackWnd

class CTrackWnd : public CWnd
{
	DECLARE_DYNAMIC(CTrackWnd)

public:
	CTrackWnd();
	virtual ~CTrackWnd();

public:
	void		AddPosition(double lat, double lon, double track);
	void		Reset() { mFront = mRear = 0; }

protected:
	void		UpdatePoints();

protected:
	//
	Position	mPos[MAX_POSITION];
	int			mRear;
	int			mFront;

	//
	Point		mPoint[MAX_POSITION];


	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


