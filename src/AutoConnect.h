/**
 *	Declaration of AutoConnect class and accompanying AutoConnectConfig class.
 *	@file	AutoConnect.h
 *	@author	hieromon@gmail.com
 *	@version	0.9.7
 *	@date	2018-11-17
 *	@copyright	MIT license.
 */

#ifndef _AUTOCONNECT_H_
#define _AUTOCONNECT_H_

#include <vector>
#include <memory>
#include <functional>
#include <DNSServer.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C" {
#include <user_interface.h>
}
using WebServerClass = ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
using WebServerClass = WebServer;
#endif
#include <EEPROM.h>
#include <PageBuilder.h>
#include "AutoConnectDefs.h"
#include "AutoConnectPage.h"
#include "AutoConnectCredential.h"
#include "AutoConnectAux.h"

/**< A type to save established credential at WiFi.begin automatically. */
typedef enum AC_SAVECREDENTIAL {
  AC_SAVECREDENTIAL_NEVER,
  AC_SAVECREDENTIAL_AUTO
} AC_SAVECREDENTIAL_t;

typedef enum AC_ONBOOTURI {
  AC_ONBOOTURI_ROOT,
  AC_ONBOOTURI_HOME
} AC_ONBOOTURI_t;

class AutoConnectConfig {
 public:
  /**
   *  AutoConnectConfig default constructor.
   *  SSID for the captive portal access point assumes AUTOCONNECT_APID which
   *  assigned from macro. Password is same as above too.
   */
  AutoConnectConfig() :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(AUTOCONNECT_APID)),
    psk(String(AUTOCONNECT_PSK)),
    channel(AUTOCONNECT_AP_CH),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}
  /**
   *  Configure by SSID for the captive portal access point and password.
   */
  AutoConnectConfig(const char* ap, const char* password, const uint8_t channel = AUTOCONNECT_AP_CH) :
    apip(AUTOCONNECT_AP_IP),
    gateway(AUTOCONNECT_AP_GW),
    netmask(AUTOCONNECT_AP_NM),
    apid(String(ap)),
    psk(String(password)),
    channel(channel),
    hidden(0),
    autoSave(AC_SAVECREDENTIAL_AUTO),
    bootUri(AC_ONBOOTURI_ROOT),
    boundaryOffset(AC_IDENTIFIER_OFFSET),
    uptime(AUTOCONNECT_STARTUPTIME),
    autoRise(true),
    autoReset(true),
    autoReconnect(false),
    immediateStart(false),
    hostName(String("")),
    homeUri(AUTOCONNECT_HOMEURI),
    staip(0U),
    staGateway(0U),
    staNetmask(0U),
    dns1(0U),
    dns2(0U) {}

  ~AutoConnectConfig() {}

  AutoConnectConfig& operator=(const AutoConnectConfig& o) {
    apip = o.apip;
    gateway = o.gateway;
    netmask = o.netmask;
    apid = o.apid;
    psk = o.psk;
    channel = o.channel;
    hidden = o.hidden;
    autoSave = o.autoSave;
    bootUri = o.bootUri;
    boundaryOffset = o.boundaryOffset;
    uptime = o.uptime;
    autoRise = o.autoRise;
    autoReset = o.autoReset;
    autoReconnect = o.autoReconnect;
    immediateStart = o.immediateStart;
    hostName = o.hostName;
    homeUri = o.homeUri;
    staip = o.staip;
    staGateway = o.staGateway;
    staNetmask = o.staNetmask;
    dns1 = o.dns1;
    dns2 = o.dns2;
    return *this;
  }

  IPAddress apip;               /**< SoftAP IP address */
  IPAddress gateway;            /**< SoftAP gateway address */
  IPAddress netmask;            /**< SoftAP subnet mask */
  String    apid;               /**< SoftAP SSID */
  String    psk;                /**< SoftAP password */
  uint8_t   channel;            /**< SoftAP used wifi channel */
  uint8_t   hidden;             /**< SoftAP SSID hidden */
  AC_SAVECREDENTIAL_t  autoSave;  /**< Auto save credential */
  AC_ONBOOTURI_t  bootUri;      /**< An uri invoking after reset */
  uint16_t  boundaryOffset;     /**< The save storage offset of EEPROM */
  int       uptime;             /**< Length of start up time */
  bool      autoRise;           /**< Automatic starting the captive portal */
  bool      autoReset;          /**< Reset ESP8266 module automatically when WLAN disconnected. */
  bool      autoReconnect;      /**< Automatic reconnect with past SSID */
  bool      immediateStart;     /**< Skips WiFi.begin(), start portal immediately */
  String    hostName;           /**< host name */
  String    homeUri;            /**< A URI of user site */
  IPAddress staip;              /**< Station static IP address */
  IPAddress staGateway;         /**< Station gateway address */
  IPAddress staNetmask;         /**< Station subnet mask */
  IPAddress dns1;               /**< Primary DNS server */
  IPAddress dns2;               /**< Secondary DNS server */
};

class AutoConnect {
 public:
  AutoConnect();
  AutoConnect(WebServerClass& webServer);
  ~AutoConnect();
  AutoConnectAux* aux(const String& uri) const;
  bool  config(AutoConnectConfig& Config);
  bool  config(const char* ap, const char* password = nullptr);
  void  home(String uri);
  bool  begin();
  bool  begin(const char* ssid, const char* passphrase = nullptr, unsigned long timeout = AUTOCONNECT_TIMEOUT);
  void  end();
  void  handleClient();
  void  handleRequest();
  WebServerClass& host();
  bool  join(AutoConnectAux& aux);
  bool  join(std::vector<std::reference_wrapper<AutoConnectAux>> aux);
  bool  on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD);

  /** For AutoConnectAux described in JSON */
#ifdef AUTOCONNECT_USE_JSON
  bool  load(const String& aux);
  bool  load(const __FlashStringHelper* aux);
  bool  load(Stream& aux, size_t bufferSize = AUTOCONNECT_JSON_BUFFER_SIZE);
  bool  _load(JsonVariant& aux);
#endif // !AUTOCONNECT_USE_JSON

  typedef std::function<bool(IPAddress)>  DetectExit_ft;
  void  onDetect(DetectExit_ft fn);
  void  onNotFound(WebServerClass::THandlerFunction fn);

 protected:
  enum _webServerAllocateType {
    AC_WEBSERVER_PARASITIC,
    AC_WEBSERVER_HOSTED
  };
  typedef enum _webServerAllocateType  AC_WEBSERVER_TYPE;
  void  _initialize();
  bool  _config();
  void  _startWebServer();
  void  _startDNSServer();
  void  _handleNotFound();
  bool  _loadAvailCredential();
  void  _stopPortal();
  bool  _classifyHandle(HTTPMethod mothod, String uri);
  PageElement*  _setupPage(String uri);

  /** Request handlers implemented by Page Builder */
  String  _induceConnect(PageArgument& args);
  String  _induceDisconnect(PageArgument& args);
  String  _induceReset(PageArgument& args);
  String  _invokeResult(PageArgument& args);

  /** For portal control */
  bool  _captivePortal();
  bool  _isIP(String ipStr);
  wl_status_t _waitForConnect(unsigned long timeout);
  void  _disconnectWiFi(bool wifiOff);

  /** Utilities */
  static uint32_t      _getChipId();
  static uint32_t      _getFlashChipRealSize();
  static String        _toMACAddressString(const uint8_t mac[]);
  static unsigned int  _toWiFiQuality(int32_t rssi);
  DetectExit_ft        _onDetectExit;
  WebServerClass::THandlerFunction _notFoundHandler;

  /** Servers which works in concert. */
  std::unique_ptr<WebServerClass> _webServer;
  std::unique_ptr<DNSServer>      _dnsServer;
  AC_WEBSERVER_TYPE               _webServerAlloc;

  /**
   *  Dynamically hold one page of AutoConnect menu.
   *  Every time a GET/POST HTTP request occurs, an AutoConnect
   *  menu page corresponding to the URI is generated.
   */
  PageBuilder*  _responsePage;
  PageElement*  _currentPageElement;

  /** Extended pages made up with AutoConnectAux */
  std::unique_ptr<AutoConnectAux> _aux;
  String  _auxLastUri;

  /** Saved configurations */
  AutoConnectConfig     _apConfig;
  struct station_config _credential;
  uint8_t       _hiddenSSIDCount;
  unsigned long _portalTimeout;

  /** The control indicators */
  bool  _rfConnect;             /**< URI /connect requested */
  bool  _rfDisconnect;          /**< URI /disc requested */
  bool  _rfReset;               /**< URI /reset requested */
  wl_status_t   _rsConnect;     /**< connection result */

  /** HTTP header information of the currently requested page. */
  String        _uri;           /**< Requested URI */
  String        _redirectURI;   /**< Redirect destination */
  IPAddress     _currentHostIP; /**< host IP address */
  String        _menuTitle;     /**< Title string of the page */

  /** PegeElements of AutoConnect site. */
  static const char _CSS_BASE[] PROGMEM;
  static const char _CSS_UL[] PROGMEM;
  static const char _CSS_ICON_LOCK[] PROGMEM;
  static const char _CSS_INPUT_BUTTON[] PROGMEM;
  static const char _CSS_INPUT_TEXT[] PROGMEM;
  static const char _CSS_TABLE[] PROGMEM;
  static const char _CSS_LUXBAR[] PROGMEM;
  static const char _ELM_HTML_HEAD[] PROGMEM;
  static const char _ELM_MENU_PRE[] PROGMEM;
  static const char _ELM_MENU_AUX[] PROGMEM;
  static const char _ELM_MENU_POST[] PROGMEM;
  static const char _PAGE_STAT[] PROGMEM;
  static const char _PAGE_CONFIGNEW[] PROGMEM;
  static const char _PAGE_OPENCREDT[] PROGMEM;
  static const char _PAGE_SUCCESS[] PROGMEM;
  static const char _PAGE_RESETTING[] PROGMEM;
  static const char _PAGE_DISCONN[] PROGMEM;
  static const char _PAGE_FAIL[] PROGMEM;
  static const char _PAGE_404[] PROGMEM;

  /** Token handlers for PageBuilder */
  String _token_CSS_BASE(PageArgument& args);
  String _token_CSS_UL(PageArgument& args);
  String _token_CSS_ICON_LOCK(PageArgument& args);
  String _token_CSS_INPUT_BUTTON(PageArgument& args);
  String _token_CSS_INPUT_TEXT(PageArgument& args);
  String _token_CSS_TABLE(PageArgument& args);
  String _token_CSS_LUXBAR(PageArgument& args);
  String _token_HEAD(PageArgument& args);
  String _token_MENU_PRE(PageArgument& args);
  String _token_MENU_AUX(PageArgument& args);
  String _token_MENU_POST(PageArgument& args);
  String _token_ESTAB_SSID(PageArgument& args);
  String _token_WIFI_MODE(PageArgument& args);
  String _token_WIFI_STATUS(PageArgument& args);
  String _token_STATION_STATUS(PageArgument& args);
  String _token_LOCAL_IP(PageArgument& args);
  String _token_SOFTAP_IP(PageArgument& args);
  String _token_GATEWAY(PageArgument& args);
  String _token_NETMASK(PageArgument& args);
  String _token_AP_MAC(PageArgument& args);
  String _token_STA_MAC(PageArgument& args);
  String _token_CHANNEL(PageArgument& args);
  String _token_DBM(PageArgument& args);
  String _token_CPU_FREQ(PageArgument& args);
  String _token_FLASH_SIZE(PageArgument& args);
  String _token_CHIP_ID(PageArgument& args);
  String _token_FREE_HEAP(PageArgument& args);
  String _token_LIST_SSID(PageArgument& args);
  String _token_HIDDEN_COUNT(PageArgument& args);
  String _token_OPEN_SSID(PageArgument& args);
  String _token_UPTIME(PageArgument& args);
  String _token_BOOTURI(PageArgument& args);

#if defined(ARDUINO_ARCH_ESP8266)
  friend class ESP8266WebServer;
#elif defined(ARDUINO_ARCH_ESP32)
  friend class WebServer;
#endif

  friend class AutoConnectAux;
};

#endif  // _AUTOCONNECT_H_
