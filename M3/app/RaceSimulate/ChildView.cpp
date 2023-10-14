
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


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
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

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;

	int nSaveDC = dc.SaveDC();
	dc.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));

	GetClientRect(rc);
	int cx = rc.left + rc.Width() / 2;
	int cy = rc.top + rc.Height() / 2;

	auto &geod = GeographicLib::Geodesic::WGS84();
	auto route = mTask.getRoute();
	double lastLat, lastLon;

	size_t startIndex = mStartTurnPoint;
	for (size_t i = 0; i < route.getTurnPointCount(); i++)
	{
		auto ptr = route.getTurnPoint(i);
		double lat = ptr->getLatitude();
		double lon = ptr->getLongitude();
		double radius = ptr->getRadius();
		double theta = ptr->getTheta();

		DrawCircle(&dc, cx, cy, lat, lon, radius);
		if (i >= startIndex)
		{
			if (i > startIndex)
			{
				double lat_target, lon_target;
				geod.Direct(lat, lon, theta, radius, lat_target, lon_target);
				DrawLine(&dc, cx, cy, lastLat, lastLon, lat_target, lon_target);

				lastLat = lat_target;
				lastLon = lon_target;
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

	//if (mPilotPosPtr)
	//	DrawLine(&dc, cx, cy, mCenterPos[GEO_LAT], mCenterPos[GEO_LON], mPilotPosPtr->getLatitude(), mPilotPosPtr->getLongitude());

	dc.RestoreDC(nSaveDC);
}

void CChildView::DrawCircle(CDC* pDC, int cx, int cy, double lat, double lon, double radius)
{
	auto &geod = GeographicLib::Geodesic::WGS84();
	double geo_x, geo_y;
	
	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], mCenterPos[GEO_LAT], lon, geo_x);
	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], lat, mCenterPos[GEO_LON], geo_y);

	int x = (int)(geo_x * mZoomRatio) * (mCenterPos[GEO_LON] < lon ? 1 : -1);
	int y = (int)(geo_y * mZoomRatio) * (mCenterPos[GEO_LAT] < lat ? -1 : 1);
	int r = (int)(radius * mZoomRatio);

	cx += x;
	cy += y;

	pDC->Ellipse(cx - r, cy - r, cx + r, cy + r);
}

void CChildView::DrawLine(CDC* pDC, int cx, int cy, double lat1, double lon1, double lat2, double lon2)
{
	auto &geod = GeographicLib::Geodesic::WGS84();
	double dx1, dx2, dy1, dy2;

	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], mCenterPos[GEO_LAT], lon1, dx1);
	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], lat1, mCenterPos[GEO_LON], dy1);
	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], mCenterPos[GEO_LAT], lon2, dx2);
	geod.Inverse(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], lat2, mCenterPos[GEO_LON], dy2);

	int x1 = (int)(dx1 * mZoomRatio) * (mCenterPos[GEO_LON] < lon1 ? 1 : -1) + cx;
	int y1 = (int)(dy1 * mZoomRatio) * (mCenterPos[GEO_LAT] < lat1 ? -1 : 1) + cy;
	int x2 = (int)(dx2 * mZoomRatio) * (mCenterPos[GEO_LON] < lon2 ? 1 : -1) + cx;
	int y2 = (int)(dy2 * mZoomRatio) * (mCenterPos[GEO_LAT] < lat2 ? -1 : 1) + cy;

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);
}


class CMyFileDialog : public CFileDialog
{
public:
	CMyFileDialog() : CFileDialog(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST, _T("XCTrack Task Files (*.xctsk)|*.xctsk;*.task|All Files (*.*)|*.*|"))
	{

	}

	virtual void OnButtonClicked(DWORD dwIDCtl)
	{
	}

	virtual void OnCheckButtonToggled(DWORD dwIDCtl, BOOL bChecked)
	{
	}

	virtual void OnControlActivating(DWORD dwIDCtl)
	{
	}

	virtual void OnItemSelected(DWORD dwIDCtl, DWORD dwIDItem)
	{
		if (dwIDCtl == 1001)
			this->SetControlState(1000, dwIDItem == 3 ? CDCS_INACTIVE : CDCS_ENABLED | CDCS_VISIBLE);
	}

	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
	{
		
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
			// calculate boundary
			auto &geod = GeographicLib::Geodesic::WGS84();
			auto route = mTask.getRoute();
			double geo_N, geo_S, geo_W, geo_E;

			for (size_t i = 0; i < route.getTurnPointCount(); i++)
			{
				auto ptr = route.getTurnPoint(i);
				if (i == 0)
				{
					geo_N = geo_S = ptr->getLatitude();
					geo_W = geo_E = ptr->getLongitude();
				}

				double lat, lon;
				geod.Direct(ptr->getLatitude(), ptr->getLongitude(), 0, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod.Direct(ptr->getLatitude(), ptr->getLongitude(), 90, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod.Direct(ptr->getLatitude(), ptr->getLongitude(), 180, ptr->getRadius(), lat, lon);
				if (lat > geo_N)
					geo_N = lat;
				if (lat < geo_S)
					geo_S = lat;
				if (lon > geo_E)
					geo_E = lon;
				if (lon < geo_W)
					geo_W = lon;
				geod.Direct(ptr->getLatitude(), ptr->getLongitude(), 270, ptr->getRadius(), lat, lon);
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

			mCenterPos[GEO_LAT] = geo_S + (geo_N - geo_S) / 2.0;
			mCenterPos[GEO_LON] = geo_W + (geo_E - geo_W) / 2.0;

			// calcualte zoom-factor
			CRect rect;
			GetClientRect(rect);
			RecalcLayout(rect.Width(), rect.Height());
			Invalidate();

			// calculate optimized path
#if 0
			mTotalDist = mOptimizedDist = 0;

			size_t points = route.getTurnPointCount();
			Eigen::VectorXd mX(points), mY(points), mR(points);
			for (size_t i = 0; i < points; i++)
			{
				auto ptr = route.getTurnPoint(i);
				mX[i] = ptr->getLongitude();
				mY[i] = ptr->getLatitude();
				mR[i] = i == 0 ? 0 : ptr->getRadius();

				if (i > 0)
				{
					double dist;
					geod.Inverse(mY[i-1], mX[i-1], mY[i], mX[i], dist);
					mTotalDist += dist;
				}
			}

			Eigen::VectorXd theta = Eigen::VectorXd::Zero(points);
			
			auto totalLength = [&](const Eigen::VectorXd &theta) -> double {
				//
				Eigen::VectorXd x_proj = Eigen::VectorXd(mX.size());
				Eigen::VectorXd y_proj = Eigen::VectorXd(mY.size());
				auto &geod = GeographicLib::Geodesic::WGS84();

				for (int i = 0; i < theta.size(); i++)
				{
					double lat, lon;
					geod.Direct(mY[i], mX[i], theta[i], mR[i], lat, lon);
					x_proj[i] = lon;
					y_proj[i] = lat;
				}

				//
				double totalDist = 0.0;

				for (int i = 0; i < theta.size() - 1; i++)
				{
					double dist;
					geod.Inverse(y_proj[i], x_proj[i], y_proj[i + 1], x_proj[i + 1], dist);
					totalDist += dist;
				}

				return totalDist;				
			};

			bfgs::optimizer<decltype(totalLength)> o(totalLength, theta);
			bfgs::result result = o.optimize();

			mTheta = std::vector<double>(points);
			for (size_t i = 0; i < mTheta.size(); i++)
				mTheta[i] = result.x[i];

			TRACE("Total Distance: %.1f Km, Optimized Distance: %.1f Km\n", mTotalDist / 1000.0, result.fval / 1000.0);
#else
			TRACE("Total Distance: %.1f Km, Optimized Distance: %.1f Km\n", route.getTotalDistance() / 1000.0, route.getOptimizedDistance() / 1000.0);
#endif

			mStartTurnPoint = 0;
			mPilotPosPtr = nullptr;
		}
		else
		{
			AfxMessageBox(_T("Open or Load Failed!"), MB_ICONSTOP);
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
	auto &geod = GeographicLib::Geodesic::WGS84();

	double geo_w, geo_h;
	geod.Inverse(mCenterPos[GEO_LAT], mBoundary[GEO_W], mCenterPos[GEO_LAT], mBoundary[GEO_E], geo_w);
	geod.Inverse(mBoundary[GEO_N], mCenterPos[GEO_LON], mBoundary[GEO_S], mCenterPos[GEO_LON], geo_h);

	double w, h;
	geod.Inverse(mBoundary[GEO_N], mBoundary[GEO_W], mBoundary[GEO_N], mBoundary[GEO_E], w);
	geod.Inverse(mBoundary[GEO_N], mBoundary[GEO_W], mBoundary[GEO_S], mBoundary[GEO_W], h);

	double scrn_w = cx - 20; // client_width - margin
	double scrn_h = cy - 20; // client_height - margin

	double ratio_1 = geo_h / geo_w;
	double ratio_2 = scrn_h / scrn_w;

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
	if (!mPilotPosPtr)
		mPilotPosPtr = std::make_shared<XcPoint>();

	if (mPilotPosPtr && mTask.getRoute().getTurnPointCount() > 0)
	{
		CRect rect;
		GetClientRect(rect);

		int cx = rect.left + rect.Width() / 2;
		int cy = rect.top + rect.Height() / 2;

		double dx = (double)point.x - (double)cx;
		double dy = (double)point.y - (double)cy;
		double dist = sqrt(dx * dx + dy * dy);
		double angle = std::atan2<double>(dx, -dy);

		// radian to degree
		const double pi = 3.14159265359;
		angle = angle * 180.0 / pi;
		if (angle < 0)
			angle = 360 + angle;
		TRACE("angle = %f\n", angle);
		// screen to geo
		dist = dist / mZoomRatio;
		TRACE("dist = %f\n", dist);
		
		// save pilot position
		auto &geod = GeographicLib::Geodesic::WGS84();
		double lat, lon;
		geod.Direct(mCenterPos[GEO_LAT], mCenterPos[GEO_LON], angle, dist, lat, lon);

		mPilotPosPtr->setLatitude(lat);
		mPilotPosPtr->setLongitude(lon);

		// optimize from new pilot position
		mTask.getRoute().optimize(mStartTurnPoint, lat, lon);

		Invalidate();
	}
}

void CChildView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (mPilotPosPtr)
	{
		size_t startPoint = mStartTurnPoint;

		if (nChar == '+')
		{
			if (startPoint + 2 < mTask.getRoute().getTurnPointCount())
				startPoint = startPoint + 1;
		}
		else if (nChar == '-')
		{
			if (startPoint > 0)
				startPoint = startPoint - 1;
		}

		if (startPoint != mStartTurnPoint)
		{
			mStartTurnPoint = startPoint;
			mTask.getRoute().optimize(mStartTurnPoint, mPilotPosPtr->getLatitude(), mPilotPosPtr->getLongitude());
			Invalidate();
		}
	}
}
