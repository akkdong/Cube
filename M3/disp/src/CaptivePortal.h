// CaptivePortal.h
//

#ifndef __CAPTIVE_PORTAL_H__
#define __CAPTIVE_PORTAL_H__

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>


////////////////////////////////////////////////////////////////////////////////////////////
//

class CaptivePortal
{
public:
  CaptivePortal();
  ~CaptivePortal();

  enum PortalState {
    PORTAL_READY,
    PORTAL_WAIT_CONNECTION,
    PORTAL_CONNECTED
  };

public:
  //
  int start();
  void stop();

  void update();

  //
  PortalState getState() { return portalState; }

  void setSSID(const char *ssid) { apSSID = ssid; }
  void setPassword(const char *pass) { apPassword = pass; }
  void setAddress(const IPAddress &ip) { apIP = ip; }

protected:
  void starSoftAP(const char *ssid, const char *pass, const IPAddress &ip, const IPAddress &gw);
  void startDNSServer(const IPAddress& ip);
  void startWebServer(const IPAddress &ip);

  void onUpdateRequest();
  void onRequestTrackLogs();
  void onDownloadTrackLog();
  void onDeleteTrackLog();
  void onRequest();

  void onWiFiEvent(WiFiEvent_t event,arduino_event_info_t info);

private:
  bool parseBoolean(String str);
  int parseInteger(String str);
  float parseFloat(String str);
  String getDateString(time_t t);

  const char * getContentType(String filename);
  bool checkExist(fs::FS & fs, String path);
  bool handleFileRead(fs::FS & fs, String path);

  void redirect(String &uri);
  void redirect(const IPAddress &ip);

protected:
  PortalState portalState;
  String apSSID;
  String apPassword;
  IPAddress apIP;
  wifi_event_id_t apEventID;

  WebServer webServer;
  DNSServer dnsServer;
};



#endif // __CAPTIVE_PORTAL_H__
