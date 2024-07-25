
#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "wizardy.h"   // .h file that stores your html page code
#include "custom_types.h"
#include "webpage.h"
#include "../global_types.h"
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
#define LOCAL_SSID ""
#define LOCAL_PASS ""
// #define USE_INTRANET

/* ------------------------------------------------------------------------------------------------
  EXTERNALS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */
char XML[4096];
char buf[32];
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;
WebServer server(80);

/* ------------------------------------------------------------------------------------------------
  GLOBAL VARIABLES
------------------------------------------------------------------------------------------------ */
extern webpageGlobalData_Type globalWebpageData_s;


/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */
void Process_StartSohMeasurment();
void Process_ToggleDischargeBattery();
void Process_ToggleChargeBattery();
void Process_SetDischargePeriod();
void Process_SetNumDischargeCycles();
void Process_SetChargePeriod();
void Process_SetNumChargeCycles();
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
  server.on("/toggleDischargeBattery", HTTP_PUT, Process_ToggleDischargeBattery);
  server.on("/toggleChargeBattery", HTTP_PUT, Process_ToggleChargeBattery);
  server.on("/setDischargePeriod", HTTP_PUT, Process_SetDischargePeriod);
  server.on("/setNumDischargeCycles", HTTP_PUT, Process_SetNumDischargeCycles);
  server.on("/setNumChargeCycles", HTTP_PUT, Process_SetNumChargeCycles);
  server.on("/setChargePeriod", HTTP_PUT, Process_SetChargePeriod);
  server.on("/setSampleRate", HTTP_PUT, Process_SetAdcSampleRate);

  // finally begin the server
  server.begin();
}

void webpage_MainFunc() {
  server.handleClient();
}


/* Start Soh Measurement */
void Process_StartSohMeasurment() {
  PRINT_LN("Start Soh Measurement button pressed: ");
  server.send(200, "text/xmlSoh", "");
  globalWebpageData_s.measureSohSwitch_b = true;
}

/* BATTERY MEASUREMENT SWITCH */
void Process_ToggleDischargeBattery() {
  PRINT_LN("Toggle Discharge Battery pressed");
  globalWebpageData_s.dischargeBatterySwitch_b = !globalWebpageData_s.dischargeBatterySwitch_b;

  PRINT("Discharge Battery status: "); PRINT_LN(globalWebpageData_s.dischargeBatterySwitch_b);
  server.send(200, "text/plain", globalWebpageData_s.dischargeBatterySwitch_b ? "1" : "0"); //Send web page
}

void Process_ToggleChargeBattery() {
  PRINT_LN("Toggle Charge Battery pressed");
  globalWebpageData_s.chargeBatterySwitch_b = !globalWebpageData_s.chargeBatterySwitch_b;

  PRINT("Charge Battery status: "); PRINT_LN(globalWebpageData_s.chargeBatterySwitch_b);
  server.send(200, "text/plain", globalWebpageData_s.chargeBatterySwitch_b ? "1" : "0"); //Send web page
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
  PRINT_LN("Set discharge cycles pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("numDischargeCycles");
  globalWebpageData_s.numDischarges_ui8 = (uint8)atoi(inputValStr.c_str());

  PRINT("Set discharge cycles to: "); PRINT_LN(globalWebpageData_s.numDischarges_ui8);
  server.send(200, "text/plain", inputValStr); //Send web page
}

void Process_SetChargePeriod() {
  PRINT_LN("Set charge period pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("chargePeriod");
  globalWebpageData_s.chargePeriod_ms_ui16 = (uint16)atoi(inputValStr.c_str());

  PRINT("Set charge period to: "); PRINT_LN(globalWebpageData_s.chargePeriod_ms_ui16);
  server.send(200, "text/plain", inputValStr); //Send web page
}

void Process_SetNumChargeCycles() {
  PRINT_LN("Set charge cycles pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("numChargeCycles");
  globalWebpageData_s.numCharges_ui8 = (uint8)atoi(inputValStr.c_str());

  PRINT("Set charge cycles to: "); PRINT_LN(globalWebpageData_s.numCharges_ui8);
  server.send(200, "text/plain", inputValStr); //Send web page
}

/* Set Num Discharge Cycles Button pressed */
void Process_SetAdcSampleRate() {
  PRINT_LN("Set sample rate pressed");
  /* Get value in input box and convert it to int */
  String inputValStr = server.arg("sampleRate");
  globalWebpageData_s.sampleRate_Hz_ui16 = (uint8)atoi(inputValStr.c_str());
  globalWebpageData_s.sampleRateChanged_b = true;
  PRINT("Set sample rate to: "); PRINT_LN(globalWebpageData_s.sampleRate_Hz_ui16);
  server.send(200, "text/plain", inputValStr); //Send web page
}


// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {
  PRINT_LN("sending web page");
  server.send(200, "text/html", PAGE_MAIN);
}


void SendXML() {
  // PRINT_LN("sending XML");
  int len = 0;  // To keep track of the length of the XML string
  len += snprintf(XML + len, sizeof(XML) - len, "<?xml version='1.0'?>\n<Data>\n");

  // Voltage and Current
  len += snprintf(XML + len, sizeof(XML) - len, "<V0>%.2f</V0>\n", globalWebpageData_s.voltageReading_mv_f32);
  len += snprintf(XML + len, sizeof(XML) - len, "<V1>%.2f</V1>\n", globalWebpageData_s.currentReading_mA_f32);

  // SOH measurement results
  len += snprintf(XML + len, sizeof(XML) - len, "<OCV>%.2f</OCV>\n", globalWebpageData_s.ocvResult_V_f32);
  len += snprintf(XML + len, sizeof(XML) - len, "<R_O>%.5f</R_O>\n", globalWebpageData_s.internalResistanceResult_Ohms_f32);
  len += snprintf(XML + len, sizeof(XML) - len, "<SOC>%.2f</SOC>\n", globalWebpageData_s.socResult_perc_f32);
  len += snprintf(XML + len, sizeof(XML) - len, "<TTS>%.1f</TTS>\n", globalWebpageData_s.ttsResult_S_f32);

  // Button states
  len += snprintf(XML + len, sizeof(XML) - len, "<DISCHARGESWITCH>%d</DISCHARGESWITCH>\n", (uint8)globalWebpageData_s.dischargeBatterySwitch_b);
  len += snprintf(XML + len, sizeof(XML) - len, "<CHARGESWITCH>%d</CHARGESWITCH>\n", (uint8)globalWebpageData_s.chargeBatterySwitch_b);
  len += snprintf(XML + len, sizeof(XML) - len, "<SOHSTATUS>%d</SOHSTATUS>\n", (uint8)globalWebpageData_s.measureSohSwitch_b);

  len += snprintf(XML + len, sizeof(XML) - len, "</Data>\n");

  if (len < sizeof(XML)) {
      server.send(200, "text/xml", XML);
  } else {
      PRINT_LN("Error: XML string too long");
      server.send(500, "text/plain", "Internal Server Error");
  }
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
