// uncomment the following line to use NimBLE library
//#define USE_NIMBLE

#ifndef __BLE_VARIO_H__
#define __BLE_VARIO_H__
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#if defined(USE_NIMBLE)

#include "NimBLECharacteristic.h"
#include "NimBLEHIDDevice.h"

#define BLEDevice                  NimBLEDevice
#define BLEServerCallbacks         NimBLEServerCallbacks
#define BLECharacteristicCallbacks NimBLECharacteristicCallbacks
#define BLEHIDDevice               NimBLEHIDDevice
#define BLECharacteristic          NimBLECharacteristic
#define BLEAdvertising             NimBLEAdvertising
#define BLEServer                  NimBLEServer

#else

#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#endif // USE_NIMBLE

#include "Print.h"

#define BLE_KEYBOARD_VERSION "0.0.4"
#define BLE_KEYBOARD_VERSION_MAJOR 0
#define BLE_KEYBOARD_VERSION_MINOR 0
#define BLE_KEYBOARD_VERSION_REVISION 4

const uint8_t KEY_LEFT_CTRL = 0x80;
const uint8_t KEY_LEFT_SHIFT = 0x81;
const uint8_t KEY_LEFT_ALT = 0x82;
const uint8_t KEY_LEFT_GUI = 0x83;
const uint8_t KEY_RIGHT_CTRL = 0x84;
const uint8_t KEY_RIGHT_SHIFT = 0x85;
const uint8_t KEY_RIGHT_ALT = 0x86;
const uint8_t KEY_RIGHT_GUI = 0x87;

const uint8_t KEY_UP_ARROW = 0xDA;
const uint8_t KEY_DOWN_ARROW = 0xD9;
const uint8_t KEY_LEFT_ARROW = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;
const uint8_t KEY_BACKSPACE = 0xB2;
const uint8_t KEY_TAB = 0xB3;
const uint8_t KEY_RETURN = 0xB0;
const uint8_t KEY_ESC = 0xB1;
const uint8_t KEY_INSERT = 0xD1;
const uint8_t KEY_PRTSC = 0xCE;
const uint8_t KEY_DELETE = 0xD4;
const uint8_t KEY_PAGE_UP = 0xD3;
const uint8_t KEY_PAGE_DOWN = 0xD6;
const uint8_t KEY_HOME = 0xD2;
const uint8_t KEY_END = 0xD5;
const uint8_t KEY_CAPS_LOCK = 0xC1;
const uint8_t KEY_F1 = 0xC2;
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB;

const uint8_t KEY_NUM_0 = 0xEA;
const uint8_t KEY_NUM_1 = 0xE1;
const uint8_t KEY_NUM_2 = 0xE2;
const uint8_t KEY_NUM_3 = 0xE3;
const uint8_t KEY_NUM_4 = 0xE4;
const uint8_t KEY_NUM_5 = 0xE5;
const uint8_t KEY_NUM_6 = 0xE6;
const uint8_t KEY_NUM_7 = 0xE7;
const uint8_t KEY_NUM_8 = 0xE8;
const uint8_t KEY_NUM_9 = 0xE9;
const uint8_t KEY_NUM_SLASH = 0xDC;
const uint8_t KEY_NUM_ASTERISK = 0xDD;
const uint8_t KEY_NUM_MINUS = 0xDE;
const uint8_t KEY_NUM_PLUS = 0xDF;
const uint8_t KEY_NUM_ENTER = 0xE0;
const uint8_t KEY_NUM_PERIOD = 0xEB;

typedef uint8_t MediaKeyReport[2];

const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1}; // Opens "My Computer" on Windows
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64}; // Media Selection
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};


//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class BLEVario : public Print, public BLEServerCallbacks, public BLECharacteristicCallbacks
{
public:
	BLEVario(std::string deviceName = "BLE Vario", std::string deviceManufacturer = "Notorious", uint8_t batteryLevel = 100);

public:
	//
	void 				begin(void);
	void 				end(void);
	void				run();

	void 				setBatteryLevel(uint8_t level);
	void 				setName(std::string deviceName);  

	bool 				isConnected(void);

	// UART
	int					available();
	int					read();
	size_t				write(uint8_t c);

	size_t				writeBuffered(uint8_t c);
	void				flush();


	// HID
	void 				setVendorID(uint16_t vid);
	void 				setProductID(uint16_t pid);
	void 				setVersion(uint16_t version);
#ifdef USE_NIMBLE
	void 				setDelay(uint32_t ms);
#endif

	void 				sendReport(KeyReport* keys);
	void 				sendReport(MediaKeyReport* keys);

	size_t 				press(uint8_t k);
	size_t 				press(const MediaKeyReport k);
	size_t 				release(uint8_t k);
	size_t 				release(const MediaKeyReport k);
	void 				releaseAll(void);

	size_t 				sendKey(uint8_t c);
	size_t 				sendKey(const MediaKeyReport c);
	size_t 				sendKey(const uint8_t *buffer, size_t size);

protected:
	virtual void 		onStarted(BLEServer *pServer) { };
	virtual void 		onConnect(BLEServer* pServer) override;
	virtual void 		onDisconnect(BLEServer* pServer) override;
	virtual void 		onWrite(BLECharacteristic* pCharacteristic) override;

	// BLEServerCallbacks
	//virtual void onConnect(BLEServer* pServer);
	//virtual void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);
	//virtual void onDisconnect(BLEServer* pServer);
	//virtual void onMtuChanged(BLEServer* pServer, esp_ble_gatts_cb_param_t* param);

	// BLECharacteristicCallbacks
	//virtual void onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param);
	//virtual void onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param);
	//virtual void onNotify(BLECharacteristic* pCharacteristic);
	//virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code);

private:
	#if defined(USE_NIMBLE)
	void               	delay_ms(uint64_t ms);
	#endif

protected:
	// Device
	std::string        	deviceName;
	std::string        	deviceManufacturer;
	uint8_t            	batteryLevel;

	//
	BLEServer*			bleServer;
	bool               	connected = false;
	bool				connected_last = false;

	// HID
	BLEHIDDevice* 		hidDevice;
	BLECharacteristic* 	inputKeyboard;
	BLECharacteristic* 	outputKeyboard;
	BLECharacteristic* 	inputMediaKeys;
	KeyReport          	_keyReport;
	MediaKeyReport     	_mediaKeyReport;

	uint16_t 			vid;
	uint16_t			pid;
	uint16_t			version;

	// UART
	BLEService* 		uartService;
	BLECharacteristic*  pTxCharacteristic;
	BLECharacteristic*  pRxCharacteristic;

	uint8_t				bufRecv[128];
	uint16_t			bufFront;
	uint16_t			bufRear;

	uint8_t				bufSend[128];
	uint16_t			sendLen;

	//
#if defined(USE_NIMBLE)
	uint32_t           	_delay_ms = 7;
#endif
};

#endif // CONFIG_BT_ENABLED
#endif // __BLE_VARIO_H__
