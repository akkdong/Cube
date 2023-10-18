// RouteSettingWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include <SD.h>


#include "board.h"
#include "RouteSettingWindow.h"
#include "Message.h"
#include "Application.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "ImageResource.h"
#include "logger.h"

#include "Route.h"



/////////////////////////////////////////////////////////////////////////////////
// class RouteSettingWindow

RouteSettingWindow::RouteSettingWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
{

}

void RouteSettingWindow::onActive()
{
    m_refreshCount = -1;
    m_lastKey = 0;

    // refresh window
    this->draw();    
}

void RouteSettingWindow::onDraw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, M5EPD_Canvas::G0);

    //
    if (taskPtr)
    {
        int cx = this->m_x + this->m_w / 2;
        int cy = this->m_y + this->m_h / 2;

        Math::real lastLat, lastLon;

        size_t startIndex = 0;
        for (size_t i = 0; i < taskPtr->getTurnPointCount(); i++)
        {
            auto ptr = taskPtr->getTurnPoint(i);
            Math::real lat = ptr->getLatitude();
            Math::real lon = ptr->getLongitude();
            Math::real radius = ptr->getRadius();
            Math::real theta = ptr->getTheta();

            drawCircle(cx, cy, lat, lon, radius);

            if (i >= startIndex)
            {
                if (i > startIndex)
                {
                    auto& proj = ptr->getProjection();
                    Math::real lat_proj = proj.getLatitude();
                    Math::real lon_proj = proj.getLongitude();

                    drawLine(cx, cy, lastLat, lastLon, lat_proj, lon_proj);

                    lastLat = lat_proj;
                    lastLon = lon_proj;
                }
                else
                {
                    /*if (mPilotPosPtr)
                    {
                        lastLat = mPilotPosPtr->getLatitude();
                        lastLon = mPilotPosPtr->getLongitude();
                    }
                    else*/
                    {
                        lastLat = lat;
                        lastLon = lon;
                    }
                }
            }
        }
    }

    // update EPD
    m_pRefCanvas->pushCanvas(0, 0, ((++m_refreshCount) % 60) == 0 ? UPDATE_MODE_GC16 : UPDATE_MODE_DU);
}

void RouteSettingWindow::drawCircle(int cx, int cy, Math::real lat, Math::real lon, Math::real radius)
{
	Math::real geo_x, geo_y;
	
	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), centerPos.getLatitude(), lon, geo_x);
	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), lat, centerPos.getLongitude(), geo_y);

	int x = (int)(geo_x * zoomRatio) * (centerPos.getLongitude() < lon ? 1 : -1);
	int y = (int)(geo_y * zoomRatio) * (centerPos.getLatitude() < lat ? -1 : 1);
	int r = (int)(radius * zoomRatio);

	cx += x;
	cy += y;

    m_pRefCanvas->drawCircle(cx, cy, r, M5EPD_Canvas::G15);
}

void RouteSettingWindow::drawLine(int cx, int cy, Math::real lat1, Math::real lon1, Math::real lat2, Math::real lon2)
{
	Math::real dx1, dx2, dy1, dy2;

	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), centerPos.getLatitude(), lon1, dx1);
	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), lat1, centerPos.getLongitude(), dy1);
	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), centerPos.getLatitude(), lon2, dx2);
	geod_inverse(centerPos.getLatitude(), centerPos.getLongitude(), lat2, centerPos.getLongitude(), dy2);

	int x1 = (int)(dx1 * zoomRatio) * (centerPos.getLongitude() < lon1 ? 1 : -1) + cx;
	int y1 = (int)(dy1 * zoomRatio) * (centerPos.getLatitude() < lat1 ? -1 : 1) + cy;
	int x2 = (int)(dx2 * zoomRatio) * (centerPos.getLongitude() < lon2 ? 1 : -1) + cx;
	int y2 = (int)(dy2 * zoomRatio) * (centerPos.getLatitude() < lat2 ? -1 : 1) + cy;

    m_pRefCanvas->drawLine(x1, y1, x2, y2, M5EPD_Canvas::G15);
}

void RouteSettingWindow::drawPilot(int cx, int cy, Math::real lat, Math::real lon)
{
}


void RouteSettingWindow::onMessage(uint32_t code, uint32_t data)
{
    switch (code)
    {
    case MSG_INVALIDATE:
        this->draw();
        break;
    }
}

void RouteSettingWindow::onTimer(uint32_t id)
{
}

void RouteSettingWindow::onTouchDown(int x, int y)
{
}

void RouteSettingWindow::onTouchMove(int x, int y)
{
}

void RouteSettingWindow::onTouchUp(int x, int y)
{
}

void RouteSettingWindow::onKeyPressed(uint32_t key)
{
    m_lastKey = key;
}

void RouteSettingWindow::onKeyLongPressed(uint32_t key)
{
    switch (key)
    {
    case EXT_KEY_LEFT:
    case EXT_KEY_RIGHT:
        break;
    case EXT_KEY_UP:
    case EXT_KEY_DOWN:
        Application::getApp()->sendMessage(MSG_FALLBACK);
        m_lastKey = 0;
        break;
    }
}

void RouteSettingWindow::onKeyReleased(uint32_t key)
{
    if (m_lastKey == 0)
        return;

    switch (key)
    {
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_PUSH:
    case EXT_KEY_UP:
        {
            static int i = 0;
            uint32_t tick = millis();
            const char * path = i == 0 ? "/Tasks/sample1.xctsk" : "/Tasks/sample2.xctsk";
            i = 1 - i;
            if (loadXcTask(path))
            {
                LOGi("1st Load & Optimize: %u ms", millis() - tick);

                tick = millis();
                this->taskPtr->optimize(0);
                LOGi("2nd Optimize: %u ms", millis() - tick);

                this->recalcLayout();
                this->draw();
            }
        }
        break;
    case EXT_KEY_LEFT:
    case EXT_KEY_RIGHT:
    case EXT_KEY_DOWN:
        break;
    }
}




bool RouteSettingWindow::loadXcTask(const char *path)
{
    std::shared_ptr<XcTask> ptr = std::make_shared<XcTask>();
    if (ptr)
    {
        File file = SD.open(path);
        if (file && ptr->load(file))
        {
            // save XcTask
            taskPtr = ptr;

            LOGi("Load Task: %s", path);
            LOGi("Total: %.1f Km, Optimized: %.1f Km", taskPtr->getTotalDistance() / 1000.0, taskPtr->getOptimizedDistance() / 1000.0);
            LOGi("Repeat count: %d", taskPtr->repeatCount);

            return true;
        }
    }

    return false;
}

#define EDGE_N      (0)
#define EDGE_E      (1)
#define EDGE_S      (2)
#define EDGE_W      (3)

#define DRAW_MARGIN (10)

void RouteSettingWindow::recalcLayout()
{
    if (!taskPtr || taskPtr->getTurnPointCount() < 2)
        return;

    // find boundary
    findBoundary(taskPtr.get(), boundary);

    centerPos.setLatitude(boundary[EDGE_S] + (boundary[EDGE_N] - boundary[EDGE_S]) / 2.0);
    centerPos.setLongitude(boundary[EDGE_W] + (boundary[EDGE_E] - boundary[EDGE_W]) / 2.0);

	Math::real boundary_w, boundary_h;
	geod_inverse(centerPos.getLatitude(), boundary[EDGE_W], centerPos.getLatitude(), boundary[EDGE_E], boundary_w);
	geod_inverse(boundary[EDGE_N], centerPos.getLongitude(), boundary[EDGE_S], centerPos.getLongitude(), boundary_h);

	//Math::real w, h;
	//geod_inverse(boundary[EDGE_N], boundary[EDGE_W], boundary[EDGE_N], boundary[EDGE_E], w);
	//geod_inverse(boundary[EDGE_N], boundary[EDGE_W], boundary[EDGE_S], boundary[EDGE_W], h);

	Math::real scrn_w = this->m_w - (DRAW_MARGIN * 2); // window_width - margin
	Math::real scrn_h = this->m_h - (DRAW_MARGIN * 2); // window_height - margin

	Math::real ratio_1 = boundary_h / boundary_w;
	Math::real ratio_2 = scrn_h / scrn_w;

	if (ratio_1 < ratio_2)
		this->zoomRatio = scrn_w / boundary_w; // fit to screen-width: w * ratio = rect.Width()
	else
		this->zoomRatio = scrn_h / boundary_h; // fit to screen-height

	int draw_w = (int)(boundary_w * this->zoomRatio);
	int draw_h = (int)(boundary_h * this->zoomRatio);

	this->offsetX = (int)((scrn_w - draw_w) / 2 + DRAW_MARGIN);
	this->offsetY = (int)((scrn_h - draw_h) / 2 + DRAW_MARGIN);
}

void RouteSettingWindow::findBoundary(XcTask *task, Math::real *border)
{
    Math::real N, S, E, W;
    Math::real lat, lon;

    for (size_t i = 0; i < task->getTurnPointCount(); i++)
    {
        auto ptr = task->getTurnPoint(i);
        if (i == 0)
        {
            N = S = ptr->getLatitude();
            E = W = ptr->getLongitude();
        }

        geod_direct(ptr->getLatitude(), ptr->getLongitude(), 0, ptr->getRadius(), lat, lon);
        N = std::max<Math::real>(N, lat);
        S = std::min<Math::real>(S, lat);
        E = std::max<Math::real>(E, lon);
        W = std::min<Math::real>(W, lon);

        geod_direct(ptr->getLatitude(), ptr->getLongitude(), 90, ptr->getRadius(), lat, lon);
        N = std::max<Math::real>(N, lat);
        S = std::min<Math::real>(S, lat);
        E = std::max<Math::real>(E, lon);
        W = std::min<Math::real>(W, lon);

        geod_direct(ptr->getLatitude(), ptr->getLongitude(), 180, ptr->getRadius(), lat, lon);
        N = std::max<Math::real>(N, lat);
        S = std::min<Math::real>(S, lat);
        E = std::max<Math::real>(E, lon);
        W = std::min<Math::real>(W, lon);

        geod_direct(ptr->getLatitude(), ptr->getLongitude(), 270, ptr->getRadius(), lat, lon);
        N = std::max<Math::real>(N, lat);
        S = std::min<Math::real>(S, lat);
        E = std::max<Math::real>(E, lon);
        W = std::min<Math::real>(W, lon);
    }

    border[EDGE_N] = N;
    border[EDGE_E] = E;
    border[EDGE_S] = S;
    border[EDGE_W] = W;
}