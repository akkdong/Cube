// main.cpp
//

#include <Arduino.h>

#include "device_defines.h"
#include "app.h"
#include "logger.h"

#include "Variometer.h"
#include "LocationParser.h"
//#include "Accumulator.h"


#define VFILTER_HARINAIR_KF2     1
#define VFILTER_HARINAIR_KF3     2
#define VFILTER_HARINAIR_KF4d    3
#define VFILTER_ROBIN_KF         4

#define USE_KALMAN_FILTER        VFILTER_HARINAIR_KF3

#if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
#include "VarioFilter_HarInAirKF2.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
#include "VarioFilter_HarInAirKF3.h"
#elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
#include "VarioFilter_HarInAirKF4d.h"
#elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
#include "VarioFilter_RobinKF.h"
#else
#error "Invalid vario kalman-filter"
#endif

class Application
{
public:
    Application();

public:
    int     begin();
    void    end();

    void    update();

protected:
    static void     FlightComputerTask(void* param);
    static void     VariometerTask(void* param);

    void            OnReadyLocationData();

protected:
    DeviceContext*  contextPtr;

    Variometer      vario;
    LocationParser  locParser;

    #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
    VarioFilter_HarInAirKF2     varioFilter;
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
    VarioFilter_HarInAirKF3     varioFilter;
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d
    VarioFilter_HarInAirKF4d    varioFilter;
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    VarioFilter_RobinKF         varioFilter;
    #endif 

    TaskHandle_t                taskFlightComputer;
    TaskHandle_t                taskVariometer;

    EventGroupHandle_t          fcEventGroup;
    QueueHandle_t               mainQueue;
    CriticalSection             contextLock;

    std::function<void (void)> locReceveProc; 
};

Application::Application()
    : contextPtr(NULL)
    , taskFlightComputer(NULL)
    , taskVariometer(NULL)
{    
    contextPtr = (DeviceContext *)ps_malloc(sizeof(DeviceContext));
}

int Application::begin()
{
    //std::function<void (void)> fpReceveProc;
    //Serial.onReceive(fpReceveProc);

    #if USE_KALMAN_FILTER == VFILTER_HARINAIR_KF2
    varioFilter.begin(400.0f, 1000.0f, 1.0f, 0);
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF3
    varioFilter.begin(400.0f, 1000.0f, 1.0f, 0.0f);
    #elif USE_KALMAN_FILTER == VFILTER_HARINAIR_KF4d

    // injects additional uncertainty depending on magnitude of acceleration
    // helps respond quickly to large accelerations while heavily filtering
    // in low acceleration situations.  Range : 0.5 - 1.5
    #define KF_ACCEL_VARIANCE_DEFAULT   100     // 50 ~ 150
    #define KF_ADAPT_DEFAULT            100     // 50 ~ 150

    varioFilter.begin(1000.0f * KF_ACCEL_VARIANCE_DEFAULT, KF_ADAPT_DEFAULT / 100.0f, 0, 0, 0);
    #elif USE_KALMAN_FILTER == VFILTER_ROBIN_KF
    varioFilter.Configure(30.0f, 4.0f, altitude);
    #endif

    vario.begin(CreateBarometer(), &varioFilter);
    locParser.begin(CreateLocationDataSource(), std::bind(&Application::OnReadyLocationData, this));

    fcEventGroup = xEventGroupCreate();

    xTaskCreate(FlightComputerTask, "FC", 4*1024, this, 2, &taskFlightComputer);
    xTaskCreate(VariometerTask, "VA", 4*1024, this, tskIDLE_PRIORITY - 1, &taskVariometer);

    return 0;
}

void Application::end()
{
}

void Application::update()
{
    // ui-loop
    //
    /*
    switch (getMessage())
    {
    case INIT_COMPLETE:
        vTaskResume(taskVariometer);
        vTaskRemove(taskFlightComputer);
        // ...

        break;

    case UPDATE_NMEA:
        
    case UPDATE_VARIO:
        break;
    }
    */
}

#define EVENT_NMEA_VALID        (1 << 1)
#define EVENT_VARIO_VALID       (1 << 2)


void Application::FlightComputerTask(void* param)
{
    Application* app = (Application *)param;
    EventGroupHandle_t evtGrpHandle = app->fcEventGroup;

    vTaskSuspend(NULL);

    while (true)
    {
        EventBits_t bits = xEventGroupWaitBits(evtGrpHandle, EVENT_NMEA_VALID | EVENT_VARIO_VALID, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & EVENT_NMEA_VALID)
        {

        }

        if (bits & EVENT_VARIO_VALID)
        {
            
        }
    }

    vTaskDelete(NULL);
}

void Application::VariometerTask(void* param)
{
    Application* app = (Application *)param;
    
    // Accumulator accVario;
    // Accumulator accPressure;

    TickType_t prevWakeTime = xTaskGetTickCount();
    while (true)
    {
        vTaskDelayUntil(&prevWakeTime, pdMS_TO_TICKS(1000 / 25));

        Variometer* vario = &app->vario;
        DeviceContext* contextPtr = app->contextPtr;

        vario->update();

        app->contextLock.enter(); // lock-context
        contextPtr->varioState.altitudeBaro = vario->getAltitudeFiltered();
        contextPtr->varioState.altitudeCalibrated = vario->getAltitudeCalibrated();
        contextPtr->varioState.pressure = vario->getPressure();
        contextPtr->varioState.temperature = vario->getTemperature();
        contextPtr->varioState.speedVertActive = vario->getVelocity();
        //contextPtr->varioState.pressureLazy = vario->getLazyPressure();
        //contextPtr->varioState.speedVertLazy = vario->getLazyVario();        
        // unlock-context

        // if (accVario.add(context->varioState.speedVertActive) > 0)
        // {
        //    context->varioState.speedVertLazy = accVario.get();
        //    context->varioState.speedVertHistory.push(accVario.get());
        //
        //    postMessage(UPDATe_VARIO)
        // }        

        // accPressure.add(context->varioState.pressure)
        // ...

        //app->beeper.setVelocity(context->varioState.speedVertActive);

        app->contextLock.leave();
    }

    vTaskDelete(NULL);
}

void Application::OnReadyLocationData()
{
    // called by HardwareSerial receive task
    locParser.update();

    if (locParser.availableLocation()) 
    {
        contextLock.enter(); // lock-context
        contextPtr->varioState.latitudeLast = contextPtr->varioState.latitude;
        contextPtr->varioState.longitudeLast = contextPtr->varioState.longitude;
        contextPtr->varioState.headingLast = contextPtr->varioState.heading;

        contextPtr->varioState.latitude = locParser.getLatitude();
        contextPtr->varioState.longitude = locParser.getLongitude();
        contextPtr->varioState.altitudeGPS = locParser.getAltitude();
        contextPtr->varioState.speedGround = locParser.getSpeed();
        contextPtr->varioState.heading = locParser.getHeading();
        contextPtr->varioState.timeCurrent = locParser.getDateTime();
        //contextPtr->varioState.altitudeGround = agl.getGroundLevel(contextPtr->varioState.latitude, contextPtr->varioState.longitude);
        contextPtr->varioState.altitudeAGL = contextPtr->varioState.altitudeGPS - contextPtr->varioState.altitudeGround;
        contextPtr->varioState.altitudeRef1 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef1;
        contextPtr->varioState.altitudeRef2 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef2;
        contextPtr->varioState.altitudeRef3 = contextPtr->varioState.altitudeGPS - contextPtr->varioSettings.altitudeRef3;        
        contextLock.leave(); // unlock-context


        // fire location-data valid event
        xEventGroupSetBits(this->fcEventGroup, EVENT_NMEA_VALID);
        locParser.resetLocation();
    }
}


void setup()
{
    Serial.begin(115200);
}

void loop()
{

}