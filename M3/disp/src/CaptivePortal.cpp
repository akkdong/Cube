// CaptivePortal.cpp
//

#include "CaptivePortal.h"
#include <SPIFFS.h>
#include <SD.h>
#include <ArduinoJson.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include "logger.h"


#define WIFI_getChipId()    (uint32_t)ESP.getEfuseMac()
#define WIFI_CHANNEL        6

#define DNS_TTL             (3600)
#define DNS_PORT            (53)



////////////////////////////////////////////////////////////////////////////////////////////
//

CaptivePortal::CaptivePortal()
    : portalState(PORTAL_READY)
    , apSSID("CaptivePortal")
    , apPassword("") 
    , apIP(4, 3, 2, 1) // or 172.217.28.1
    , apEventID(0)
{

}

CaptivePortal::~CaptivePortal()
{
    if (portalState != PORTAL_READY)
    {
        stop();
    }
}

int CaptivePortal::start()
{
    if (portalState != PORTAL_READY)
        return -1;

	starSoftAP(apSSID.c_str(), apPassword.c_str(), apIP, apIP);
	startDNSServer(apIP);
	startWebServer(apIP);

    portalState = PORTAL_WAIT_CONNECTION;
	return 0;
}

void CaptivePortal::stop()
{
    if (portalState != PORTAL_READY)
    {
        webServer.close();
        dnsServer.stop();

        WiFi.removeEvent(apEventID);
        WiFi.softAPdisconnect(true);

        portalState = PORTAL_READY;
        apEventID = 0;
    }
}

void CaptivePortal::update()
{
    if (portalState != PORTAL_READY)
    {
	    dnsServer.processNextRequest();
	    webServer.handleClient();
    }
}

void CaptivePortal::starSoftAP(const char *ssid, const char *pass, const IPAddress &ip, const IPAddress &gw)
{
	// event handler
    apEventID = WiFi.onEvent(std::bind(&CaptivePortal::onWiFiEvent, this, std::placeholders::_1, std::placeholders::_2));
    LOGd("WiFi.onEvent() => %u", apEventID);

	// set WIFI mode to AP
    WiFi.persistent(false);
	bool ret = WiFi.mode(WIFI_MODE_AP);
	LOGd("WiFi.mode() => %d", ret);

	// configure AP with specfic ip/gw/mask
	const IPAddress mask(255, 255, 255, 0);
	ret = WiFi.softAPConfig(ip, gw, mask);
	LOGd("WiFi.softAPConfig(%s, %s, %s) => %d", ip.toString().c_str(), gw.toString().c_str(), mask.toString(), ret);

	// start AP with given ssid, password, channel
	ret = WiFi.softAP(ssid, pass && pass[0] ? pass : NULL, WIFI_CHANNEL);
	LOGd("WiFi.softAP() => %d", ret);

	// Disable AMPDU RX on the ESP32 WiFi to fix a bug on Android
	/*
	esp_wifi_stop();
	esp_wifi_deinit();
	wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
	my_config.ampdu_rx_enable = false;
	esp_wifi_init(&my_config);
	esp_wifi_start();
	vTaskDelay(100 / portTICK_PERIOD_MS);  // Add a small delay  
	*/
}

void CaptivePortal::startDNSServer(const IPAddress& ip)
{
  // set TTL for DNS & start it
  dnsServer.setTTL(DNS_TTL);
  bool ret = dnsServer.start(DNS_PORT, "*", ip);
  LOGd("DNSServer.start() => %d", ret);
}

void CaptivePortal::redirect(const IPAddress &ip)
{
	String location = String("http://") + ip.toString();

	redirect(location);
}

void CaptivePortal::redirect(String &uri)
{
	LOGd("-> Redirect to %s", uri.c_str());

    webServer.sendHeader("Location", uri, true); // @HTTPHEAD send redirect
    webServer.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webServer.client().stop(); // Stop is needed because we sent no content length
}

void CaptivePortal::startWebServer(const IPAddress &ip)
{
	// Portal connection validation pages
	webServer.on(Uri("/connecttest.txt"), [this, ip]() { LOGd("/connecttest.txt"); redirect(ip); });	// windows 11 captive portal workaround
	webServer.on(Uri("/wpad.dat"), [this]() { LOGd("/wpad.dat"); webServer.send(404); });				// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)
	webServer.on(Uri("/generate_204"), [this, ip]() { LOGd("/generate_204"); redirect(ip); });		   	// android captive portal redirect
	webServer.on(Uri("/redirect"), [this, ip]() { LOGd("/redirect"); redirect(ip); });			   	    // microsoft redirect
	webServer.on(Uri("/hotspot-detect.html"), [this, ip]() { LOGd("/hotspot-detect.html"); redirect(ip); });  // apple call home
	webServer.on(Uri("/canonical.html"), [this, ip]() { LOGd("/canonical.html"); redirect(ip); });	   	// firefox captive portal call home
	webServer.on(Uri("/success.txt"), [this, ip]() { LOGd("/success.txt"); webServer.send(200); });	    // firefox captive portal call home
	webServer.on(Uri("/ncsi.txt"), [this, ip]() { LOGd("/ncsi.txt"); redirect(ip); });			   	    // windows call home

    // Portal service pages
	webServer.on(UriBraces("/Update/{}"), HTTP_POST, std::bind(&CaptivePortal::onUpdateRequest, this));
	webServer.on(Uri("/TrackLogs/list"), HTTP_GET, std::bind(&CaptivePortal::onRequestTrackLogs, this));
	webServer.on(UriBraces("/TrackLogs/{}"), HTTP_GET, std::bind(&CaptivePortal::onDownloadTrackLog, this));
	webServer.on(UriBraces("/TrackLogs/{}"), HTTP_DELETE, std::bind(&CaptivePortal::onDeleteTrackLog, this));
    // Poral default request handler
	webServer.onNotFound(std::bind(&CaptivePortal::onRequest, this));

	//
	webServer.begin();
    LOGv("WebServer.begin()");
}


void CaptivePortal::onUpdateRequest()
{
    String target = "/" + webServer.pathArg(0);
    LOGv("Update target: %s", target.c_str());
    
    if (!SPIFFS.exists(target))
    {
        webServer.send(404, "text/plain", "File Not Found");
        LOGw("File Not Exist: %s", target.c_str());
    }
    else
    {
        //
        const size_t capacity = JSON_OBJECT_SIZE(29) + 1024;
        DynamicJsonDocument doc(capacity);

        // read
        {
            File file = SPIFFS.open(target, FILE_READ);
            if (file)
            {
                deserializeJson(doc, file);
                file.close();
            }
        }

        // update
        for (int i = 0; i < webServer.args(); i++)
        {
            LOGd("%s: %s", webServer.argName(i).c_str(), webServer.arg(i).c_str());

            String name = webServer.argName(i);
            String value = webServer.arg(i);
            JsonVariant var = doc[name];

            if (! var.isNull())
            {
                if (var.is<bool>())
                    doc[name] = parseBoolean(value);
                else if (var.is<int>())
                    doc[name] = parseInteger(value);
                else if (var.is<float>())
                    doc[name] = parseFloat(value);
                else
                    doc[name] = value;
            }
            else
            {
                doc[name] = value;
            }
        }

        // save
        {
            File file = SPIFFS.open(target, FILE_WRITE);
            size_t size = 0;

            if (file)
            {
                size = serializeJson(doc, file);
                file.close();

                LOGd("Serialize %d bytes", size);
            }

            if (size != 0)
            {
                webServer.send(200, "text/plain", "OK");
                LOGv("Update Success.");
            }
            else
            {
                webServer.send(400, "text/plain", "File Write Failed");
                LOGw("Update Failed! : %s", target.c_str());
            }
        }
    }
}

void CaptivePortal::onRequestTrackLogs()
{
    #if 0 // write header directory. but not permitted
    const char * hdr =  "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "Accept-Ranges: none\r\n"
                        "Connection: close\r\n"
                        "\r\n";
    webServer._chunked = true;
    webServer._currentClientWrite(hdr, strlen(hdr));

    webServer.sendContent("[");
    #else
    webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webServer.send(200, "application/json", "[");
    #endif

    File dir = SD.open("/TrackLogs");
    if (dir.isDirectory())
    {
        File file = dir.openNextFile();
        int count = 0;

        LOGv("List up TrakcLogs");
        while (file)
        {
            if (! file.isDirectory() && file.size() > 0)
            {
                String name(file.name());

                if (name.endsWith(".igc"))
                {
                    // { "name": "xxx.igc", "size": nnn, "date": "YYYY-MM-DD hh:mm:ssZ" }
                    String igc;

                    // 
                    if (count > 0)
                        igc += ",";
                        
                    igc += "{\"name\":\"";
                    igc += name;
                    igc += "\",\"size\":";
                    igc += file.size();
                    igc += ",\"date\":\"";
                    igc += getDateString(file.getLastWrite());
                    igc += "\"}";

                    webServer.sendContent(igc);
                    LOGv(igc.c_str());

                    count += 1;
                }
            }

            file = dir.openNextFile();
            }
        }

    webServer.sendContent("]");
    webServer.sendContent("");
}

void CaptivePortal::onDownloadTrackLog()
{
    String target = "/TrackLogs/" + webServer.pathArg(0);
    LOGv("handleFileDowload: %s", target.c_str());

    this->handleFileRead(SD, target);
}

void CaptivePortal::onDeleteTrackLog()
{
    String target = "/TrackLogs/" + webServer.pathArg(0);
    LOGv("handleFileDelete: %s", target.c_str());

    if (SD.remove(target.c_str()))
    {
        webServer.send(200, "text/plain", "OK");
        LOGv("TrackLog deleted: %s", target.c_str());
    }
    else
    {
        webServer.send(400, "text/plain", "File Delete Failed");
        LOGw("TrackLog delete Failed: %s", target.c_str());
    }
}

void CaptivePortal::onRequest()
{
    this->handleFileRead(SPIFFS, webServer.uri());
}

void CaptivePortal::onWiFiEvent(WiFiEvent_t event,arduino_event_info_t info)
{
    LOGv("[WiFi] onEvent: %s", WiFi.eventName(event));
    switch (event) 
    {
    case ARDUINO_EVENT_WIFI_READY:
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        portalState = PORTAL_CONNECTED;
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        portalState = PORTAL_WAIT_CONNECTION;
        // reason ??
        //  WIFI_REASON_CONNECTION_FAIL
        //  WIFI_REASON_AUTH_FAIL
        //  WIFI_REASON_AUTH_EXPIRE
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        LOGv( "[WiFi] STA IP: %s", IPAddress(info.wifi_ap_staipassigned.ip.addr).toString().c_str());
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        break;
    case ARDUINO_EVENT_WIFI_FTM_REPORT:
        break;
    case ARDUINO_EVENT_ETH_START:
        break;
    case ARDUINO_EVENT_ETH_STOP:
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        break;
    case ARDUINO_EVENT_WPS_ER_PIN:
        break;
    case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP:
        break;
    case ARDUINO_EVENT_SC_SCAN_DONE:
        break;
    case ARDUINO_EVENT_SC_FOUND_CHANNEL:
        break;
    case ARDUINO_EVENT_SC_GOT_SSID_PSWD:
        break;
    case ARDUINO_EVENT_SC_SEND_ACK_DONE:
        break;
    case ARDUINO_EVENT_PROV_INIT:
        break;
    case ARDUINO_EVENT_PROV_DEINIT:
        break;
    case ARDUINO_EVENT_PROV_START:
        break;
    case ARDUINO_EVENT_PROV_END:
        break;
    case ARDUINO_EVENT_PROV_CRED_RECV:
        break;
    case ARDUINO_EVENT_PROV_CRED_FAIL:
        break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        break;
    }
}


const char * CaptivePortal::getContentType(String filename)
{
    if (filename.endsWith(".htm") || filename.endsWith(".html"))
        return "text/html";

    if (filename.endsWith(".css"))
        return "text/css";

    if (filename.endsWith(".js"))
        return "application/javascript";

    if (filename.endsWith(".json"))
        return "application/json";

    if (filename.endsWith(".png"))
        return "image/png";

    if (filename.endsWith(".gif"))
        return "image/gif";

    if (filename.endsWith(".jpg") || filename.endsWith(".jpeg"))
        return "image/jpeg";

    if (filename.endsWith(".ico"))
        return "image/x-icon";

    if (filename.endsWith(".igc"))
        return "application/octet-stream";

    if (filename.endsWith(".xml"))
        return "text/xml";

    if (filename.endsWith(".zip"))
        return "application/x-zip";

    if (filename.endsWith(".gz"))
        return "application/x-gzip";

    if (filename.endsWith(".pdf"))
        return "application/x-pdf";

    return "text/plain";
}

bool CaptivePortal::checkExist(fs::FS & fs, String path)
{
    bool exist = false;

    File file = fs.open(path, "r");
    if (! file.isDirectory())
        exist = true;
    file.close();

    return exist;
}

bool CaptivePortal::handleFileRead(fs::FS & fs, String path)
{
    LOGv("handleFileRead: %s", path.c_str());
    if (path.endsWith("/"))
        path += "index.html";

    String contentType = getContentType(path);
    LOGd("  contentType: %s", contentType.c_str());
    String path_gz = path + ".gz";

    //if (checkExist(fs, path_gz) || checkExist(fs, path))
    if (fs.exists(path_gz) || fs.exists(path))
    {
        //if (checkExist(fs, path_gz))
        if (fs.exists(path_gz))
            path = path + ".gz";

        LOGd("  Open: %s", path.c_str());
        File file = fs.open(path, "r");
        if (file)
        {
            webServer.streamFile(file, contentType);
            file.close();

            return true;
        }
        
        webServer.send(400, "text/plain", "File Open Failed");
        LOGw("  File open failed!: %s", path.c_str());    
    }
    else
    {
        webServer.send(404, "text/plain", "File Not Found");
        LOGv("  File not exist! : %s", path.c_str());
    }

    return false;
}


bool CaptivePortal::parseBoolean(String str)
{
    if (str == "true")
        return true;

    return false;
}

int CaptivePortal::parseInteger(String str)
{
    return atoi(str.c_str());
}

float CaptivePortal::parseFloat(String str)
{
    return atof(str.c_str());
}

String CaptivePortal::getDateString(time_t t)
{
    struct tm * _tm;
    _tm = gmtime(&t);

    char str[32]; // YYYY-MM-DD hh:mm:ssZ
    String date;
    
    sprintf(str, "%d-%d-%d %d:%d:%dZ", 
        _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday,
        _tm->tm_hour, _tm->tm_min, _tm->tm_sec);

    return String(str);
}


