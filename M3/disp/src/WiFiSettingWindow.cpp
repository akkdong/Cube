// WiFiSettingWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>
#include <WiFiManager.h>

#include "board.h"
#include "WiFiSettingWindow.h"
#include "Message.h"
#include "Application.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "ImageResource.h"
#include "logger.h"
#include "WebService.h"


#define CREDENTIAL_OFFSET           (0)




/////////////////////////////////////////////////////////////////////////////////
// class WiFiSettingWindow


WiFiSettingWindow::WiFiSettingWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
{
}



void WiFiSettingWindow::onActive()
{
    m_refreshCount = -1;
    m_lastKey = 0;

    // start wifi-task
    xTaskCreatePinnedToCore(WiFiTask, "WiFi", 8 * 1024, this, 1, &taskWiFi, 0);

    // refresh window
    this->draw();
}

void WiFiSettingWindow::onDraw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, M5EPD_Canvas::G0);

    // draw state of each step
    //
    // Step0
    // Step1
    // Step2
    // Step3

    m_pRefCanvas->setTextDatum(TL_DATUM);
    m_pRefCanvas->setTextSize(48);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
    m_pRefCanvas->drawString("WiFi Setting", 100, 40);

    // update EPD
    m_pRefCanvas->pushCanvas(0, 0, UPDATE_MODE_GC16);
}

void WiFiSettingWindow::onMessage(uint32_t code, uint32_t data)
{
    switch (code)
    {
    case MSG_WIFI_START:
        break;
    case MSG_WIFI_STOP:
        break;
    }
}

void WiFiSettingWindow::onTimer(uint32_t id)
{
}

void WiFiSettingWindow::onTouchDown(int x, int y)
{
}

void WiFiSettingWindow::onTouchMove(int x, int y)
{
}

void WiFiSettingWindow::onTouchUp(int x, int y)
{
}

void WiFiSettingWindow::onKeyPressed(uint32_t key)
{
    m_lastKey = key;
}

void WiFiSettingWindow::onKeyLongPressed(uint32_t key)
{
    switch (key)
    {
    case EXT_KEY_LEFT:
    case EXT_KEY_RIGHT:
        break;
    case EXT_KEY_UP:
        break;
    case EXT_KEY_DOWN:
        Application::getApp()->sendMessage(MSG_FALLBACK);
        break;
    }

    m_lastKey = 0;
}

void WiFiSettingWindow::onKeyReleased(uint32_t key)
{
    if (m_lastKey == 0)
        return;

    LOGv("[WiFi] Key released: %u", key);    
    switch (key)
    {
    case KEY_LEFT:
    case EXT_KEY_LEFT:
        break;
    case KEY_RIGHT:
    case EXT_KEY_RIGHT:
        break;
    case EXT_KEY_UP:
        LOGv("Start WiFi");
        vTaskResume(taskWiFi);
        break;
    case EXT_KEY_DOWN:
        // nop
        break;
    }
}

void WiFiSettingWindow::WiFiTask(void *param)
{
    // jump to main-routine
    ((WiFiSettingWindow *)param)->WiFiTask();

    // end of task
    vTaskDelete(NULL);
}



WiFiManager wifiManager;

void configModeCallback (WiFiManager *myWiFiManager) 
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void WiFiSettingWindow::WiFiTask()
{
    // suspend this task and wait resume
    LOGv("Suspend WiFiSetting window");
    vTaskSuspend(NULL);
    LOGv("Resume WiFiSetting window");


    Serial.println("Start WiFiManager");
    //wifiManager.resetSettings();
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setConfigPortalBlocking(true);
    wifiManager.setShowPassword(true);
    wifiManager.setShowStaticFields(true);
    wifiManager.setShowInfoUpdate(false);
    wifiManager.setDarkMode(true);
    wifiManager.autoConnect("Cube-S3", "1234567890");
    Serial.println("  --> Done!");

    WebServiceClass& webService = WebServiceClass::getService();
    webService.on("/Update/{}", HTTP_POST, WebServiceClass::onUpdateRequest);
    webService.on("/TrackLogs/list", HTTP_GET, WebServiceClass::onRequestTrackLogs);
    webService.on("/TrackLogs/{}", HTTP_GET, WebServiceClass::onDownloadTrackLog);
    webService.on("/TrackLogs/{}", HTTP_DELETE, WebServiceClass::onDeleteTrackLog);
    webService.on("/", WebServiceClass::onRequest);
    webService.onNotFound(WebServiceClass::onRequest);
    webService.begin();

    while (1)
    {
        webService.handleClient();
    }

    #if 0
    AutoConnectConfig Config("Cube-M3", "1234567890", 10 * 60* 1000);
    Config.boundaryOffset = CREDENTIAL_OFFSET;   
    wifiPortal.config(Config);

    // Now! start wifi-portal
    //
    // Step0:
    //   - show registered credentials
    //   - show operation: 
    //        "connect to registered AP"
    //        or
    //        connect "Cube-M3" & connect to desired AP
    //   - "start" or "start after clear"
    //
    // Step2:
    //   - wait for connection to the AP
    //   - connected or timeout(10 minutes)
    //
    // Step3:
    //   - connected: show ip & start web-service
    //   - timeout: restart after 10 seconds

    WebServiceClass& webService = WebServiceClass::getService();
    webService.on("/", [] () {
        char content[] = "Hello, world";
        WebServiceClass::getService().send(200, "text/plain", content);
    });
    //webService.on("/Update/{}", HTTP_POST, WebServiceClass::onUpdateRequest);
    //webService.on("/TrackLogs/list", HTTP_GET, WebServiceClass::onRequestTrackLogs);
    //webService.on("/TrackLogs/{}", HTTP_GET, WebServiceClass::onDownloadTrackLog);
    //webService.on("/TrackLogs/{}", HTTP_DELETE, WebServiceClass::onDeleteTrackLog);
    //webService.on("/", WebServiceClass::onRequest);
    //webService.onNotFound(WebServiceClass::onRequest);

    if (!wifiPortal.begin())
    {
        LOGv("WiFi portal timeout!! reboot after 10 seconds~");

        // draw count-down : 10 seconds
        // restart after 10 seconds
        //
        // BUT! just restart in here
        ESP.restart();
    }

    // AP connected!!
    LOGv("WiFI connected: %s", WiFi.localIP().toString().c_str());

    // set web-server pages: web-server started in portal
    //WebServer& server = wifiPortal.host();


    while(1)
    {
        // handle portal-events
        wifiPortal.handleClient();
    }
    #endif
}
