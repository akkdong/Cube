// WebService.h
//

#include <WebServer.h>



///////////////////////////////////////////////////////////////////////////////////
// class WebServiceClass

class WebServiceClass : public WebServer
{
public:
    WebServiceClass();
    virtual ~WebServiceClass();

public:
    void begin() override;

protected:
    const char * getContentType(String filename);
    bool checkExist(fs::FS & fs, String path);
    bool handleFileRead(fs::FS & fs, String path);

    void onUpdateRequest();
    void onRequestTrackLogs();
    void onDownloadTrackLog();
    void onDeleteTrackLog();
    void onRequest();    
};
