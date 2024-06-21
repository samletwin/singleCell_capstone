
#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "wizardy.h"   // .h file that stores your html page code
#include "custom_types.h"
#include "webpage.h"
/* from: https://github.com/KrisKasprzak/ESP32_WebPage/blob/main/WebPageUpdate.ino */


/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL
#define PRINT_LN(test) Serial.println(test);
#define PRINT(test) Serial.print(test);
#else
#define PRINT_LN(test) 
#define PRINT(test) 
#endif

#define AP_SSID "ESP32"
#define AP_PASS "lolstorm"
#define LOCAL_SSID "sam_google_wifi"
#define LOCAL_PASS "Birds2002"
#define USE_INTRANET

/* ------------------------------------------------------------------------------------------------
  EXTERNALS
------------------------------------------------------------------------------------------------ */
uint32_t SensorUpdate = 0;

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
char XML[2048];
char buf[32];
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;
WebServer server(80);

/* ------------------------------------------------------------------------------------------------
  GLOBAL VARIABLES
------------------------------------------------------------------------------------------------ */
webpageGlobalData_s globalWebpageData_s;


/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void Process_StartSohMeasurment();
void Process_ToggleCurrentMeasurement();
void Process_SetDischargePeriod();
void Process_SetNumDischargeCycles();
void Process_SetAdcSampleRate();
void SendWebsite();
void SendXML();
void printWifiStatus();

/* ------------------------------------------------------------------------------------------------
  FUNCTION DEFINITIONS
------------------------------------------------------------------------------------------------ */
void webpage_Setup() {
  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  // disableCore1WDT();

  /* Init struct */
  globalWebpageData_s = (webpageGlobalData_s){0};
  globalWebpageData_s.currentReading_mA_f32 = 0.0;

  // just an update to progress
  PRINT_LN("starting server");

#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    PRINT(".");
  }
  PRINT("IP address: "); PRINT_LN(WiFi.localIP());
#endif
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  PRINT("IP address: "); PRINT_LN(WiFi.softAPIP());
#endif

  printWifiStatus();
  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  // upon ESP getting /ProcessButton_0 string, ESP will execute the ProcessButton_0 function
  // same notion for the following .on calls
  // add as many as you need to process incoming strings from your web page
  // as you can imagine you will need to code some javascript in your web page to send such strings
  // this process will be documented in the SuperMon.h web page code
  server.on("/startSohMeasurment", HTTP_PUT, Process_StartSohMeasurment);
  server.on("/toggleCurrentMeasurement", HTTP_PUT, Process_ToggleCurrentMeasurement);
  server.on("/setDischargePeriod", HTTP_PUT, Process_SetDischargePeriod);
  server.on("/setNumDischargeCycles", HTTP_PUT, Process_SetNumDischargeCycles);
  server.on("/setSampleRate", HTTP_PUT, Process_SetAdcSampleRate);

  // finally begin the server
  server.begin();
}

void webpage_MainFunc() {
  server.handleClient();
}

/* Function to enable the soh button after measurement has finished*/
void webpage_SetSohButtonStatus(bool enable) {
  if (enable == true) {
    PRINT_LN("Enabling the SOH Button");
  }
  else {
    PRINT_LN("Disabling the SOH Button");
  }
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");
  sprintf(buf, "<SOH>%d</SOH>\n", (uint8)enable);
  strcat(XML, "</Data>\n");
  server.send(200, "text/xmlSoh", XML);
}

/* Start Soh Measurement */
void Process_StartSohMeasurment() {
  PRINT_LN("Start Soh Measurement button pressed: ");
  server.send(200, "text/xmlSoh", "");
  globalWebpageData_s.measureSohSwitch_b = true;
  webpage_SetSohButtonStatus(false); /* Disable button - re-enable after measurement complete*/
}

/* BATTERY MEASUREMENT SWITCH */
void Process_ToggleCurrentMeasurement() {
  PRINT_LN("Toggle current measurement pressed");
  globalWebpageData_s.measureCurrentSwitch_b = !globalWebpageData_s.measureCurrentSwitch_b;

  PRINT("Current measurement status: "); PRINT_LN(globalWebpageData_s.measureCurrentSwitch_b);
  server.send(200, "text/plain", ""); //Send web page
}

/* Set Discharge Period Button pressed */
void Process_SetDischargePeriod() {
  PRINT_LN("Set discharge period pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("dischargePeriod");
  globalWebpageData_s.dischargePeriod_ms_ui16 = (uint16)atoi(inputValStr.c_str());

  PRINT("Set discharge period to: "); PRINT_LN(globalWebpageData_s.dischargePeriod_ms_ui16);
  server.send(200, "text/plain", inputValStr); //Send web page
}

/* Set Num Discharge Cycles Button pressed */
void Process_SetNumDischargeCycles() {
  PRINT_LN("Set discharge period pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("numDischargeCycles");
  globalWebpageData_s.numDischarges_ui8 = (uint8)atoi(inputValStr.c_str());

  PRINT("Set discharge cycles to: "); PRINT_LN(globalWebpageData_s.numDischarges_ui8);
  server.send(200, "text/plain", inputValStr); //Send web page
}

/* Set Num Discharge Cycles Button pressed */
void Process_SetAdcSampleRate() {
  PRINT_LN("Set sample rate pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("sampleRate");
  globalWebpageData_s.sampleRate_Hz_ui16 = (uint8)atoi(inputValStr.c_str());

  PRINT("Set sample rate to: "); PRINT_LN(globalWebpageData_s.sampleRate_Hz_ui16);
  server.send(200, "text/plain", inputValStr); //Send web page
}


// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {
  PRINT_LN("sending web page");
  server.send(200, "text/html", PAGE_MAIN);
}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML() {

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  // send Volts0
  sprintf(buf, "<V0>%d</V0>\n", (uint32)globalWebpageData_s.voltageReading_mv_f32);
  strcat(XML, buf);
  // send Volts1
  sprintf(buf, "<V1>%d</V1>\n", (uint32)globalWebpageData_s.currentReading_mA_f32);
  strcat(XML, buf);

  // send OCV
  sprintf(buf, "<OCV>%.2f</OCV>\n", globalWebpageData_s.ocvResult_V_f32);
  strcat(XML, buf);
  // send R_O
  sprintf(buf, "<R_O>%.5f</R_O>\n", globalWebpageData_s.internalResistanceResult_Ohms_f32);
  strcat(XML, buf);
  // send SOC
  sprintf(buf, "<SOC>%.2f</SOC>\n", globalWebpageData_s.socResult_perc_f32);
  strcat(XML, buf);
  // send TTS
  sprintf(buf, "<TTS>%d</TTS>\n", globalWebpageData_s.ttsResult_S_ui32);
  strcat(XML, buf);

  // show led0 status
  if (globalWebpageData_s.measureCurrentSwitch_b == true) {
    strcat(XML, "<SWITCH>1</SWITCH>\n");
  }
  else {
    strcat(XML, "<SWITCH>0</SWITCH>\n");
  }

  strcat(XML, "</Data>\n");
  server.send(200, "text/xml", XML);
}

void printWifiStatus() {

  // print the SSID of the network you're attached to:
  PRINT("SSID: ");
  PRINT_LN(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  PRINT("IP Address: ");
  PRINT_LN(ip);

  // print the received signal strength:
  uint8 rssi = WiFi.RSSI();
  PRINT("signal strength (RSSI):");
  PRINT(rssi);
  PRINT_LN(" dBm");
  // print where to go in a browser:
  PRINT("Open http://");
  PRINT_LN(ip);
}
