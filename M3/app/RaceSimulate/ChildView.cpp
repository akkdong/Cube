
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "RaceSimulate.h"
#include "ChildView.h"
#include <fstream>
#include <GeographicLib/Geodesic.hpp>

#undef min
#undef max
#include "bfgs.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_OPEN		(1001)
#define IDC_ZOOMIN		(1002)
#define IDC_ZOOMOUT		(1003)
#define IDC_NEXT		(1004)
#define IDC_PREV		(1005)


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(IDC_OPEN, OnFileOpen)
	ON_COMMAND(IDC_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDC_ZOOMOUT, OnZoomOut)
	ON_COMMAND(IDC_NEXT, OnMoveNext)
	ON_COMMAND(IDC_PREV, OnMovePrev)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE; 
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndOpen.Create(_T("Open"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(CPoint(10, 10), CSize(120, 32)), this, IDC_OPEN);
	m_wndZoomIn.Create(_T("Zoom In"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(10, 50), CSize(120, 32)), this, IDC_ZOOMIN);
	m_wndZoomOut.Create(_T("Zoom OUt"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(10, 90), CSize(120, 32)), this, IDC_ZOOMOUT);
	m_wndNext.Create(_T("Next"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(10, 130), CSize(120, 32)), this, IDC_NEXT);
	m_wndPrev.Create(_T("Prev"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(10, 170), CSize(120, 32)), this, IDC_PREV);

	return 0;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;

	CPen penStart(PS_SOLID, 2, RGB(255, 0, 0));
	CPen penFinish(PS_SOLID, 2, RGB(0, 0, 255));
	CPen penNormal(PS_SOLID, 1, RGB(0, 0, 0));
	int nSaveDC = dc.SaveDC();
	dc.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));

	GetClientRect(rc);
	int cx = rc.left + rc.Width() / 2;
	int cy = rc.top + rc.Height() / 2;

	Math::real lastLat, lastLon;

	size_t startIndex = mStartTurnPoint;
	for (size_t i = 0; i < mTask.getTurnPointCount(); i++)
	{
		auto ptr = mTask.getTurnPoint(i);
		Math::real lat = ptr->getLatitude();
		Math::real lon = ptr->getLongitude();
		Math::real radius = ptr->getRadius();
		Math::real theta = ptr->getTheta();

		CPen* pOldPen = nullptr;
		if (i == 0)
			pOldPen = dc.SelectObject(&penStart);
		else if (i + 1 == mTask.getTurnPointCount())
			pOldPen = dc.SelectObject(&penFinish);
		DrawCircle(&dc, cx, cy, lat, lon, radius);
		if (pOldPen)
			dc.SelectObject(pOldPen);

		if (i >= startIndex)
		{
			if (i > startIndex)
			{
				auto& proj = ptr->getProjection();
				Math::real lat_proj = proj.getLatitude();
				Math::real lon_proj = proj.getLongitude();

				DrawLine(&dc, cx, cy, lastLat, lastLon, lat_proj, lon_proj);

				lastLat = lat_proj;
				lastLon = lon_proj;
			}
			else
			{
				if (mPilotPosPtr)
				{
					lastLat = mPilotPosPtr->getLatitude();
					lastLon = mPilotPosPtr->getLongitude();
				}
				else
				{
					lastLat = lat;
					lastLon = lon;
				}
			}
		}
	}

	// draw boundary
	//{
	//	DrawLine(&dc, cx, cy, mBoundary[GEO_N], mBoundary[GEO_W], mBoundary[GEO_N], mBoundary[GEO_E]);
	//	DrawLine(&dc, cx, cy, mBoundary[GEO_N], mBoundary[GEO_E], mBoundary[GEO_S], mBoundary[GEO_E]);
	//	DrawLine(&dc, cx, cy, mBoundary[GEO_S], mBoundary[GEO_E], mBoundary[GEO_S], mBoundary[GEO_W]);
	//	DrawLine(&dc, cx, cy, mBoundary[GEO_S], mBoundary[GEO_W], mBoundary[GEO_N], mBoundary[GEO_W]);
	//}

	if (mPilotPosPtr)
	{
		// draw pilot position
		DrawPilot(&dc, cx, cy, mPilotPosPtr->getLatitude(), mPilotPosPtr->getLongitude());
	}

	dc.RestoreDC(nSaveDC);
}

void CChildView::DrawCircle(CDC* pDC, int cx, int cy, Math::real lat, Math::real lon, Math::real radius)
{
	Math::real geo_x, geo_y;
	
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), mCenterPos.getLatitude(), lon, geo_x);
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), lat, mCenterPos.getLongitude(), geo_y);

	int x = (int)(geo_x * mZoomRatio) * (mCenterPos.getLongitude() < lon ? 1 : -1);
	int y = (int)(geo_y * mZoomRatio) * (mCenterPos.getLatitude() < lat ? -1 : 1);
	int r = (int)(radius * mZoomRatio);

	cx += x;
	cy += y;

	pDC->Ellipse(cx - r, cy - r, cx + r, cy + r);
}

void CChildView::DrawLine(CDC* pDC, int cx, int cy, Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2)
{
	Math::real dx1, dx2, dy1, dy2;

	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), mCenterPos.getLatitude(), lon1, dx1);
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), lat1, mCenterPos.getLongitude(), dy1);
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), mCenterPos.getLatitude(), lon2, dx2);
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), lat2, mCenterPos.getLongitude(), dy2);

	int x1 = (int)(dx1 * mZoomRatio) * (mCenterPos.getLongitude() < lon1 ? 1 : -1) + cx;
	int y1 = (int)(dy1 * mZoomRatio) * (mCenterPos.getLatitude() < lat1 ? -1 : 1) + cy;
	int x2 = (int)(dx2 * mZoomRatio) * (mCenterPos.getLongitude() < lon2 ? 1 : -1) + cx;
	int y2 = (int)(dy2 * mZoomRatio) * (mCenterPos.getLatitude() < lat2 ? -1 : 1) + cy;

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);
}

void CChildView::DrawPilot(CDC* pDC, int cx, int cy, Math::real lat, Math::real lon)
{
	Math::real geo_x, geo_y;

	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), mCenterPos.getLatitude(), lon, geo_x);
	geod_inverse(mCenterPos.getLatitude(), mCenterPos.getLongitude(), lat, mCenterPos.getLongitude(), geo_y);

	int x = (int)(geo_x * mZoomRatio) * (mCenterPos.getLongitude() < lon ? 1 : -1);
	int y = (int)(geo_y * mZoomRatio) * (mCenterPos.getLatitude() < lat ? -1 : 1);

	cx += x;
	cy += y;

	pDC->MoveTo(cx + 10, cy + 10);
	pDC->LineTo(cx - 10, cy - 10);
	pDC->MoveTo(cx + 10, cy - 10);
	pDC->LineTo(cx - 10, cy + 10);
}


class CMyFileDialog : public CFileDialog
{
public:
	CMyFileDialog() : CFileDialog(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST, _T("XCTrack Task Files (*.xctsk)|*.xctsk;*.task|All Files (*.*)|*.*|"))
	{

	}

	virtual void OnItemSelected(DWORD dwIDCtl, DWORD dwIDItem)
	{
		if (dwIDCtl == 1001)
			this->SetControlState(1000, dwIDItem == 3 ? CDCS_INACTIVE : CDCS_ENABLED | CDCS_VISIBLE);
	}
};


void CChildView::OnFileOpen()
{
#if 1
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST, _T("XCTrack Task Files (*.xctsk)|*.xctsk;*.task|All Files (*.*)|*.*|"));
#else
	CMyFileDialog dlg;
	dlg.StartVisualGroup(9999, _T("Option"));
	dlg.AddComboBox(1001);
	dlg.AddControlItem(1001, 1, _T("A"));
	dlg.AddControlItem(1001, 2, _T("B"));
	dlg.AddControlItem(1001, 3, _T("C"));
	dlg.AddControlItem(1001, 4, _T("D"));
	dlg.SetSelectedControlItem(1001, 3);
	dlg.EndVisualGroup();
	dlg.AddCheckButton(1000, _T("Check Here"), FALSE);
#endif

	if (dlg.DoModal() == IDOK)
	{
		std::ifstream fin;

		int len = WideCharToMultiByte(CP_ACP, 0, dlg.GetPathName(), -1, NULL, 0, NULL, NULL);
		std::vector<char> fname(len + 1);
		WideCharToMultiByte(CP_ACP, 0, dlg.GetPathName(), -1, fname.data(), len, NULL, NULL);
		fin.open(fname.data());

		if (fin.is_open() && mTask.load(fin))
		{
#if CALC_REPEATCOUNT
			TRACE("OPTIMIZE repeat-count : %d\n", mTask.repeatCount);
#endif
			// calculate boundary
			Math::real geo_N, geo_S, geo_W, geo_E;

			for (size_t i = 0; i < mTask.getTurnPointCount(); i++)
			{
				auto ptr = mTask.getTurnPoint(i);
				if (i == 0)
				{
					geo_N = geo_S = ptr->getLatitude();
					geo_W = geo_E = ptr->getLongitude();
				}

				Math::real lat, lon;
				geod_direct(ptr->getLatitude(), ptr->getLongitude(), 0, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod_direct(ptr->getLatitude(), ptr->getLongitude(), 90, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod_direct(ptr->getLatitude(), ptr->getLongitude(), 180, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod_direct(ptr->getLatitude(), ptr->getLongitude(), 270, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
			}

			mBoundary[GEO_W] = geo_W; // left
			mBoundary[GEO_N] = geo_N; // top
			mBoundary[GEO_E] = geo_E; // right
			mBoundary[GEO_S] = geo_S; // bottom

			mCenterPos.setLatitude(geo_S + (geo_N - geo_S) / 2.0);
			mCenterPos.setLongitude(geo_W + (geo_E - geo_W) / 2.0);

			// calcualte zoom-factor
			CRect rect;
			GetClientRect(rect);
			RecalcLayout(rect.Width(), rect.Height());

			mStartTurnPoint = 0;
			mPilotPosPtr = nullptr;

			TRACE("Total Distance: %.1f Km, Optimized Distance: %.1f Km\n", mTask.getTotalDistance() / 1000.0, mTask.getOptimizedDistance() / 1000.0);
			Invalidate();
		}
		else
		{
			AfxMessageBox(_T("Open or Load Failed!"), MB_ICONSTOP);
		}
	}
}

void CChildView::OnZoomIn()
{
	mZoomRatio += mZoomRatio * 0.1;
	Invalidate();
}

void CChildView::OnZoomOut()
{
	mZoomRatio -= mZoomRatio * 0.1;
	Invalidate();
}

void CChildView::OnMoveNext()
{
	if (mPilotPosPtr)
	{
		size_t startPoint = mStartTurnPoint;

		if (startPoint + 2 < mTask.getTurnPointCount())
			startPoint = startPoint + 1;

		if (startPoint != mStartTurnPoint)
		{
			Optimize(startPoint, mPilotPosPtr->getLatitude(), mPilotPosPtr->getLongitude());
			Invalidate();
		}
	}
}

void CChildView::OnMovePrev()
{
	if (mPilotPosPtr)
	{
		size_t startPoint = mStartTurnPoint;

		if (startPoint > 0)
			startPoint = startPoint - 1;

		if (startPoint != mStartTurnPoint)
		{
			Optimize(startPoint, mPilotPosPtr->getLatitude(), mPilotPosPtr->getLongitude());
			Invalidate();
		}
	}
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	RecalcLayout(cx, cy);
}

void CChildView::RecalcLayout(int cx, int cy)
{
	Math::real geo_w, geo_h;
	geod_inverse(mCenterPos.getLatitude(), mBoundary[GEO_W], mCenterPos.getLatitude(), mBoundary[GEO_E], geo_w);
	geod_inverse(mBoundary[GEO_N], mCenterPos.getLongitude(), mBoundary[GEO_S], mCenterPos.getLongitude(), geo_h);

	Math::real w, h;
	geod_inverse(mBoundary[GEO_N], mBoundary[GEO_W], mBoundary[GEO_N], mBoundary[GEO_E], w);
	geod_inverse(mBoundary[GEO_N], mBoundary[GEO_W], mBoundary[GEO_S], mBoundary[GEO_W], h);

	Math::real scrn_w = cx - 20; // client_width - margin
	Math::real scrn_h = cy - 20; // client_height - margin

	Math::real ratio_1 = geo_h / geo_w;
	Math::real ratio_2 = scrn_h / scrn_w;

	if (ratio_1 < ratio_2)
		mZoomRatio = scrn_w / geo_w; // fit to screen-width: w * ratio = rect.Width()
	else
		mZoomRatio = scrn_h / geo_h; // fit to screen-height

	int draw_w = (int)(geo_w * mZoomRatio);
	int draw_h = (int)(geo_h * mZoomRatio);

	mDrawOffset.x = (LONG)((scrn_w - draw_w) / 2 + 10);
	mDrawOffset.y = (LONG)((scrn_h - draw_h) / 2 + 10);
}


void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (mTask.getTurnPointCount() < 2)
		return;

	// hit-test: calculate geo-position
	Math::real lat, lon;
	{
		CRect rect;
		GetClientRect(rect);

		int cx = rect.left + rect.Width() / 2;
		int cy = rect.top + rect.Height() / 2;

		Math::real dx = (Math::real)point.x - (Math::real)cx;
		Math::real dy = (Math::real)point.y - (Math::real)cy;
		Math::real dist = sqrt(dx * dx + dy * dy);
		Math::real angle = std::atan2<Math::real>(dx, -dy);

		// radian to degree
		const Math::real pi = 3.14159265359;
		angle = angle * 180.0 / pi;
		if (angle < 0)
			angle = 360 + angle;
		// screen to geo
		dist = dist / mZoomRatio;

		// calculate pilot position
		geod_direct(mCenterPos.getLatitude(), mCenterPos.getLongitude(), angle, dist, lat, lon);
	}


	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		mCenterPos.setLatitude(lat);
		mCenterPos.setLongitude(lon);
	}
	else
	{
		if (!mPilotPosPtr)
		{
			mPilotPosPtr = std::make_shared<XcBasePoint>(lat, lon);

			auto ptr = mTask.getTurnPoint(mStartTurnPoint + 1);
			mPilotInCylinder = ptr->inside(lat, lon); // should I exit or enter ?

			TRACE(">\n");
			TRACE(">\n");
			TRACE("> START %s (%f, %f)\n", ptr->getName(), ptr->getLatitude(), ptr->getLongitude());
			TRACE(">\n");
			TRACE(">\n");
		}
		else
		{
			mPilotPosPtr->setLatitude(lat);
			mPilotPosPtr->setLongitude(lon);
		}

		if (mPilotPosPtr && mStartTurnPoint + 1 < mTask.getTurnPointCount())
		{
			// check cylinder hit
			auto ptr = mTask.getTurnPoint(mStartTurnPoint + 1);
			bool inside = ptr->inside(lat, lon);

			if (mPilotInCylinder != inside) // case of inbound or outbound
			{
				if (mStartTurnPoint + 2 == mTask.getTurnPointCount())
				{
					// final
					++mStartTurnPoint;

					TRACE("*\n");
					TRACE("*\n");
					TRACE("* GOAL !!!!\n");
					TRACE("*\n");
					TRACE("*\n");
				}
				else
				{
					// move to next turn-point
					OnMoveNext();

					auto ptr = mTask.getTurnPoint(mStartTurnPoint + 1);
					mPilotInCylinder = ptr->inside(lat, lon);

					TRACE(">\n");
					TRACE(">\n");
					TRACE("> NEXT %s (%f, %f)\n", ptr->getName(), ptr->getLatitude(), ptr->getLongitude());
					TRACE(">\n");
					TRACE(">\n");
				}
			}

			if (mStartTurnPoint + 1 < mTask.getTurnPointCount())
			{
				// optimize from new pilot position
				Optimize(mStartTurnPoint, lat, lon);
			}
		}
	}

	Invalidate();
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CChildView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CChildView::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_UP:
			OnZoomIn();
			break;
		case VK_DOWN:
			OnZoomOut();
			break;
		case VK_RIGHT:
			OnMoveNext();
			break;
		case VK_LEFT:
			OnMovePrev();
			break;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CChildView::Optimize(size_t startPoint, Math::real lat, Math::real lon)
{
	mStartTurnPoint = startPoint;
	mTask.optimize(mStartTurnPoint, lat, lon);
	TRACE("Optimized Distance: %.1f Km\n", mTask.getOptimizedDistance() / 1000.0);
#if CALC_REPEATCOUNT
	TRACE("OPTIMIZE repeat-count : %d\n", mTask.repeatCount);
#endif
}
