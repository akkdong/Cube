// WebService.cpp
//

#include <ArduinoJson.h>
#include <SD.h>
#include <SPIFFS.h>
#include "WebService.h"
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>


///////////////////////////////////////////////////////////////////////////////////
// Helper APIs

bool parseBoolean(String str)
{
    if (str == "true")
        return true;

    return false;
}

int parseInteger(String str)
{
    return atoi(str.c_str());
}

float parseFloat(String str)
{
    return atof(str.c_str());
}

String getDateString(time_t t)
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




///////////////////////////////////////////////////////////////////////////////////
// class WebServiceClass implements

WebServiceClass::WebServiceClass() : WebServer(80)
{
}

WebServiceClass::~WebServiceClass()
{
}


const char * WebServiceClass::getContentType(String filename)
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

bool WebServiceClass::checkExist(fs::FS & fs, String path)
{
    bool exist = false;

    File file = fs.open(path, "r");
    if (! file.isDirectory())
        exist = true;
    file.close();

    return exist;
}

bool WebServiceClass::handleFileRead(fs::FS & fs, String path)
{
    Serial.print("handleFileRead: ");
    Serial.println(path);
    if (path.endsWith("/"))
        path += "index.html";

    String contentType = getContentType(path);
    Serial.println("  contentType: " + contentType);
    String path_gz = path + ".gz";

    //if (checkExist(fs, path_gz) || checkExist(fs, path))
    if (fs.exists(path_gz) || fs.exists(path))
    {
        //if (checkExist(fs, path_gz))
        if (fs.exists(path_gz))
            path = path + ".gz";

        Serial.printf("  Open: %s\n", path.c_str());
        File file = fs.open(path, "r");
        if (file)
        {
            this->streamFile(file, contentType);
            file.close();

            return true;
        }
        
        Serial.println("  File open failed!");    
        this->send(400, "text/plain", "File Open Failed");
    }
    else
    {
        Serial.println("  File not exist!");
        this->send(404, "text/plain", "File Not Found");
    }

    return false;
}

void WebServiceClass::onUpdateRequest()
{
    String target = "/" + this->pathArg(0);

    if (! SPIFFS.exists(target))
    {
        Serial.printf("File Not Exist: %s\n", target);

        this->send(404, "text/plain", "File Not Found");
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
        for (int i = 0; i < this->args(); i++)
        {
            Serial.printf("%s: %s\n", this->argName(i).c_str(), this->arg(i).c_str());

            String name = this->argName(i);
            String value = this->arg(i);
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

                Serial.printf("Serialize %d bytes\n", size);
            }

            if (size != 0)
            {
                Serial.printf("Update Success.\n");

                this->send(200, "text/plain", "OK");
            }
            else
            {
                Serial.printf("Update Failed!\n");
                this->send(400, "text/plain", "File Write Failed");
            }
        }
    }
}

void WebServiceClass::onRequestTrackLogs()
{
    const char * hdr =  "HTTP/1.1 200 OK\r\n"
                        "Content-Type: application/json\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "Accept-Ranges: none\r\n"
                        "Connection: close\r\n"
                        "\r\n";
    this->_chunked = true;
    this->_currentClientWrite(hdr, strlen(hdr));

    this->sendContent("[");
    File dir = SD.open("/TrackLogs");
    if (dir.isDirectory())
    {
        File file = dir.openNextFile();
        int count = 0;

        while (file)
        {
            if (! file.isDirectory() && file.size() > 0)
            {
                String name(file.name());

                if (name.endsWith(".igc"))
                {
                    // { "name": "xxx", "size": nnn, "date": ddd }
                    String igc;

                    // 
                    if (count > 0)
                        igc += ",";
                        
                    igc += "{\"name\":\"";
                    igc += name.substring(11); // remove prefix: /TrackLogs/
                    igc += "\",\"size\":";
                    igc += file.size();
                    igc += ",\"date\":\"";
                    igc += getDateString(file.getLastWrite());
                    igc += "\"}";

                    this->sendContent(igc);
                    Serial.println(igc);

                    count += 1;
                }
            }

            file = dir.openNextFile();
            }
        }

    this->sendContent("]");
    this->sendContent("");
}

void WebServiceClass::onDownloadTrackLog()
{
    String target = "/TrackLogs/" + this->pathArg(0);

    this->handleFileRead(SD, target);
}

void WebServiceClass::onDeleteTrackLog()
{
    String target = "/TrackLogs/" + this->pathArg(0);

    Serial.print("handleFileDelete: ");
    Serial.println(target);

    if (SD.remove(target.c_str()))
    {
        this->send(200, "text/plain", "OK");
        }
    else
    {
        this->send(400, "text/plain", "File Delete Failed");
    }
}

void WebServiceClass::onRequest()
{
    this->handleFileRead(SPIFFS, this->uri());
}



void WebServiceClass::begin()
{
    this->on(UriBraces("/Update/{}"), HTTP_POST, std::bind(&WebServiceClass::onUpdateRequest, this));
    this->on(Uri("/TrackLogs/list"), HTTP_GET, std::bind(&WebServiceClass::onRequestTrackLogs, this));
    this->on(UriBraces("/TrackLogs/{}"), HTTP_GET, std::bind(&WebServiceClass::onDownloadTrackLog, this));
    this->on(UriBraces("/TrackLogs/{}"), HTTP_DELETE, std::bind(&WebServiceClass::onDeleteTrackLog, this));
    this->onNotFound(std::bind(&WebServiceClass::onRequest, this)); 

    WebServer::begin();
}