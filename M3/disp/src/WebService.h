// WebService.h
//

#include <WebServer.h>



///////////////////////////////////////////////////////////////////////////////////
// class WebServiceClass

class WebServiceClass : public WebServer
{
protected:
    WebServiceClass();

public:
    static WebServiceClass& getService();

public:
    static const char * getContentType(String filename);
    static bool checkExist(fs::FS & fs, String path);
    static bool handleFileRead(fs::FS & fs, String path);

    static void onUpdateRequest();
    static void onRequestTrackLogs();
    static void onDownloadTrackLog();
    static void onDeleteTrackLog();
    static void onRequest();    

    static void deleteAllCredentials();
};
