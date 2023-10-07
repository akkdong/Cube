// WiFiSettingWindow.cpp
//

#include <Arduino.h>
#include <M5EPD_Canvas.h>

#include "board.h"
#include "WiFiSettingWindow.h"
#include "Message.h"
#include "Application.h"
#include "DeviceContext.h"
#include "DeviceRepository.h"
#include "ImageResource.h"
#include "logger.h"


#define CREDENTIAL_OFFSET           (0)




/////////////////////////////////////////////////////////////////////////////////
// class WiFiSettingWindow


WiFiSettingWindow::WiFiSettingWindow(M5EPD_Canvas *pRefCanvas)
    : Window(pRefCanvas, 0, 0, LCD_WIDTH, LCD_HEIGHT)
    , btnReboot(pRefCanvas, 0, 0, 0, 0)
    , m_lastTouch(-1)
{
}



void WiFiSettingWindow::onActive()
{
    m_refreshCount = -1;
    m_lastKey = 0;

    // start wifi-task
    //xTaskCreatePinnedToCore(WiFiTask, "WiFi", 8 * 1024, this, 1, &taskWiFi, 0);

    btnReboot.setStyle(1);
    btnReboot.setImage(ImageResource_btn_reboot_64x64, 64, 64);
    btnReboot.setLabel("Reboot");
    btnReboot.setTextSize(32);
    btnReboot.move(760, 360, 160, 140);
    btnReboot.show(true);

    // refresh window
    this->draw();
    this->setTimer(1, 1000);

    // start WIFI(captive portal)
    LOGv("Start WiFi: invoke captive-portal");
    Application::getApp()->sendMessage(MSG_WIFI_START);
}

void WiFiSettingWindow::onClose()
{
    LOGv("Stop WiFi: terminate captive-portal");
    Application::getApp()->sendMessage(MSG_WIFI_STOP);
}

void WiFiSettingWindow::onDraw()
{
    // erase bkgnd
    m_pRefCanvas->fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, M5EPD_Canvas::G0);

    // draw wifi logo
    int x = 102, y = 62;
    m_pRefCanvas->pushImage(x - 96 / 2, y - 96 / 2, 96, 96, ImageResource_wifi_logo_m_96x96);
    // draw "Instruction" string
    m_pRefCanvas->setTextDatum(CL_DATUM);
    m_pRefCanvas->setTextSize(48);
    m_pRefCanvas->setTextColor(M5EPD_Canvas::G15);
    m_pRefCanvas->drawString("Instruction", x + 96 / 2 + 8, y);

    // draw instruction
    DeviceContext *contextPtr = DeviceRepository::instance().getContext();
    char sz[128];

    x = 82; y = 146;
    m_pRefCanvas->setTextSize(32);
    m_pRefCanvas->drawString("1. Connect to WiFi", x, y);
    x = 110; y = 196;
    sprintf(sz, "SSID: %s, Password: %s", contextPtr->deviceDefault.wifiSSID, contextPtr->deviceDefault.wifiPassword[0] ? contextPtr->deviceDefault.wifiPassword : "");
    m_pRefCanvas->drawString(sz, x, y);

    x = 82; y = 280;
    m_pRefCanvas->drawString("2. Open Configuration Page", x, y);
    x = 110; y = 330;
    if (contextPtr->deviceState.ipAddr[0])
        sprintf(sz, "http://%s/", contextPtr->deviceState.ipAddr);
    else
        strcpy(sz, "IP: <un-assigned>");
    m_pRefCanvas->drawString(sz, x, y);

    // draw wifi state
    x = 66; y = 468;
    sprintf(sz, "WiFi State: %s", getWfiFiState(contextPtr));
    m_pRefCanvas->setTextSize(48);
    m_pRefCanvas->drawString(sz, x, y);
    m_pRefCanvas->drawFastHLine(64, 498, 696 - 64, M5EPD_Canvas::G15);
    m_pRefCanvas->drawFastHLine(64, 499, 696 - 64, M5EPD_Canvas::G15);
    m_pRefCanvas->drawFastHLine(64, 500, 696 - 64, M5EPD_Canvas::G15);

    // draw animation
    x = 700; y = 24;
    m_pRefCanvas->pushImage(x, y, 96, 96, ImageResource_mobile_m_96x96);
    x = 840;
    m_pRefCanvas->pushImage(x, y, 96, 96, ImageResource_laptop_m_96x96);
    x = 780; y = 28;
    switch (aniIndex)
    {
    case 1:
        m_pRefCanvas->pushImage(x, y, 42, 74, ImageResource_radio_strength_1_42x74);
        break;
    case 2:
        m_pRefCanvas->pushImage(x, y, 42, 74, ImageResource_radio_strength_2_42x74);
        break;
    case 3:
        m_pRefCanvas->pushImage(x, y, 42, 74, ImageResource_radio_strength_3_42x74);
        break;
    }

    // draw buttin
    btnReboot.draw();

    // update EPD
    m_pRefCanvas->pushCanvas(0, 0, ((++m_refreshCount) % 60) == 0 ? UPDATE_MODE_GC16 : UPDATE_MODE_DU);
}

const char * WiFiSettingWindow::getWfiFiState(DeviceContext *contextPtr)
{
    //arduino_event_id_t event = static_cast<arduino_event_id_t>(contextPtr->deviceState.wifiState);
    //return WiFi.eventName(event);
    switch(contextPtr->deviceState.wifiState)
    {
    case 1:
        return "AP Ready";
    case 2:
        return "Connected";
    case 0:
    default:
        return "WiFi Off";
    }
}

void WiFiSettingWindow::onMessage(uint32_t code, uint32_t data)
{
    switch (code)
    {
    case MSG_WIFI_STATE_CHANGED:
    case MSG_INVALIDATE:
        this->draw();
        break;
    }
}

void WiFiSettingWindow::onTimer(uint32_t id)
{
    this->aniIndex = (this->aniIndex + 1) % 4;
    this->draw();
}

void WiFiSettingWindow::onTouchDown(int x, int y)
{
    int index = hitTest(x, y);
    LOGv("WiFiSettingWindow hit-test: (%d, %d) -> %d", x, y, index);

    if (index == 0)
    {
        LOGv("Touch reboot button");
        btnReboot.setActive(true);
        Application::getApp()->sendMessage(MSG_INVALIDATE);
        m_lastTouch = 0;
    }
}

void WiFiSettingWindow::onTouchMove(int x, int y)
{
}

void WiFiSettingWindow::onTouchUp(int x, int y)
{
    int index = hitTest(x, y);
    if (index == m_lastTouch)
        rebootDevice();

    btnReboot.setActive(false);
    Application::getApp()->sendMessage(MSG_INVALIDATE);
    m_lastTouch = -1;
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
    case EXT_KEY_DOWN:
        #if USE_FALLBACK
        Application::getApp()->sendMessage(MSG_FALLBACK);
        #else
        // restart device
        rebootDevice();
        #endif
        break;
    }
}

void WiFiSettingWindow::onKeyReleased(uint32_t key)
{
    if (m_lastKey == 0)
        return;

    LOGv("[WiFi] Key released: %u", key);    
    switch (key)
    {
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_PUSH:
    case EXT_KEY_UP:
    case EXT_KEY_LEFT:
    case EXT_KEY_RIGHT:
    case EXT_KEY_DOWN:
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


#if 0
WiFiManager wifiManager;
WebServiceClass webService;

void configModeCallback (WiFiManager *myWiFiManager) 
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}
#endif

void WiFiSettingWindow::WiFiTask()
{
    #if 0
    // suspend this task and wait resume
    LOGv("Suspend WiFiSetting window");
    vTaskSuspend(NULL);
    LOGv("Resume WiFiSetting window");

    //
    // WiFi State
    //
    // DISCONNECTED
    // STA_CONNECTING
    // STA_CONNECTED(WIFI_CONNECTED)
    // AP_WAIT
    // AP_CONNECTED(WIFI_CONNECTED)
    //
    // AP Config: SSID, AP_PASSWORD, STATIC_IP/GW/SN
    // STA Config: SSID, PASSWORD
    //
    //
    // WIFI: mode(AP, STA), SSID, PASSWORD
    //
    //

    //typedef void (*WiFiEventCb)(arduino_event_id_t event);
    //typedef std::function<void(arduino_event_id_t event, arduino_event_info_t info)> WiFiEventFuncCb;
    //typedef void (*WiFiEventSysCb)(arduino_event_t *event);
    //

    WiFi.onEvent(/*WiFiEventCb*/ [] (arduino_event_id_t evt) {});
    WiFi.onEvent(/*WiFiEventFuncCb*/[] (arduino_event_id_t evt, arduino_event_info_t info) {});
    WiFi.onEvent(/*WiFiEventSysCb*/[] (arduino_event_t *evt) {});

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

    //WebServiceClass& webService = WebServiceClass::getService();
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
    #endif
}

int WiFiSettingWindow::hitTest(int x, int y)
{
    if (btnReboot.pointInWidget(x, y))
        return 0;

    return -1;
}

void WiFiSettingWindow::rebootDevice()
{
    LOGi("Restart device!");
    
    delay(100);
    ESP.restart();    
}
