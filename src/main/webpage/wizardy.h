const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<title>Web Page Update Demo</title>

  <style>
    table {
      position: relative;
      width:100%;
      border-spacing: 0px;
    }
    tr {
      border: 1px solid white;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
    }
    th {
      height: 20px;
      padding: 3px 15px;
      background-color: #343a40;
      color: #FFFFFF !important;
      }
    td {
      height: 20px;
       padding: 3px 15px;
    }
    .tabledata {
      font-size: 24px;
      position: relative;
      padding-left: 5px;
      padding-top: 5px;
      height:   25px;
      border-radius: 5px;
      color: #FFFFFF;
      line-height: 20px;
      transition: all 200ms ease-in-out;
      background-color: #00AA00;
    }
    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display:inline;
    }
    .navbar {
      width: 100%;
      height: 50px;
      margin: 0;
      padding: 10px 0px;
      background-color: #FFF;
      color: #000000;
      border-bottom: 5px solid #293578;
    }
    .fixed-top {
      position: fixed;
      top: 0;
      right: 0;
      left: 0;
      z-index: 1030;
    }
    .navtitle {
      float: left;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 50px;
      font-weight: bold;
      line-height: 50px;
      padding-left: 20px;
    }
   .navheading {
     position: fixed;
     left: 60%;
     height: 50px;
     font-family: "Verdana", "Arial", sans-serif;
     font-size: 20px;
     font-weight: bold;
     line-height: 20px;
     padding-right: 20px;
   }
   .navdata {
      justify-content: flex-end;
      position: fixed;
      left: 70%;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      font-weight: bold;
      line-height: 20px;
      padding-right: 20px;
   }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .heading {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: normal;
      font-size: 28px;
      text-align: left;
    }
  
    .btn {
      background-color: #444444;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .foot {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      position: relative;
      height:   30px;
      text-align: center;   
      color: #AAAAAA;
      line-height: 20px;
    }
    .container {
      max-width: 1800px;
      margin: 0 auto;
    }
    table tr:first-child th:first-child {
      border-top-left-radius: 5px;
    }
    table tr:first-child th:last-child {
      border-top-right-radius: 5px;
    }
    table tr:last-child td:first-child {
      border-bottom-left-radius: 5px;
    }
    table tr:last-child td:last-child {
      border-bottom-right-radius: 5px;
    }
    .battery-status {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      margin-bottom: 20px;
      color: #000000;
    }
  </style>

  <body style="background-color: #efefef" onload="process()">
  
    
  
    <main class="container" style="margin-top:70px">
      <div class="category">Battery Readings</div>
      <div class="battery-status" id="batteryStatus">Checking battery connection...</div>
      <div style="border-radius: 10px !important;">
      <table style="width:50%">
      <colgroup>
        <col span="1" style="background-color:rgb(230,230,230); width: 20%; color:#000000 ;">
        <col span="1" style="background-color:rgb(200,200,200); width: 15%; color:#000000 ;">
        <col span="1" style="background-color:rgb(180,180,180); width: 15%; color:#000000 ;">
      </colgroup>
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <tr>
        <th colspan="1"><div class="heading">Pin</div></th>
        <th colspan="1"><div class="heading">Reading</div></th>
      </tr>
      <tr>
        <td><div class="bodytext">Battery Voltage (mV)</div></td>
        <td><div class="tabledata" id = "v0"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">Battery Current (mA)</div></td>
        <td><div class="tabledata" id = "v1"></div></td>
      </table>
    </div>
    <br>
    <div class="category">SOH/SOC Results</div>
      <div style="border-radius: 10px !important;">
      <table style="width:50%">
      <colgroup>
        <col span="1" style="background-color:rgb(230,230,230); width: 20%; color:#000000 ;">
        <col span="1" style="background-color:rgb(200,200,200); width: 15%; color:#000000 ;">
        <col span="1" style="background-color:rgb(180,180,180); width: 15%; color:#000000 ;">
      </colgroup>
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <tr>
        <th colspan="1"><div class="heading">Measurement</div></th>
        <th colspan="1"><div class="heading">Value</div></th>
      </tr>
      <tr>
        <td><div class="bodytext">Internal Resistance (mOhms)</div></td>
        <td><div class="tabledata" id = "r_o"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">SOC (%)</div></td>
        <td><div class="tabledata" id = "soc"></div></td>
      </table>
    </div>
    <br>

    <div class="category">Battery Controls</div>
    <br>
      <div class="bodytext">Start SOH Measurement: </div>
      <button type="button" class = "btn" id = "btnSoh" onclick="startSohMeasurement()">Start</button>
      <br>
      <div class="bodytext">Discharge Battery Switch: </div>
      <button type="button" class = "btn" id = "btn1" onclick="toggleDischargeBattery()">Toggle</button>
      <br>
      <div class="bodytext">Charge Battery Switch: </div>
      <button type="button" class="btn" id="btnCharge" onclick="toggleChargeBattery()">Turn ON</button>
      </div>
      
      <br>
      <div class="bodytext">Discharge Period (in ms): </div>
      <input type="number" min="1" step="1" max="65535" id="dischargePeriod" name="dischargePeriodInput">
      <button type="button" class="btn" id="setDischargePeriod" onclick="setDischargePeriod()">Set</button>
      <br>
      
      
      <div class="bodytext">Charge Period (in ms): </div>
      <input type="number" min="1" step="1" max="65535" id="chargePeriod" name="chargePeriodInput">
      <button type="button" class="btn" id="setChargePeriod" onclick="setChargePeriod()">Set</button>
      <br>
      <div class="bodytext">Number of Cycles: </div>
      <input type="number" min="0" step="1" max="255" id="numCycles" name="numCyclesInput">
      <button type="button" class="btn" id="setNumCycles" onclick="setNumCycles()">Set</button>
      </div>
      <br>
      <div class="bodytext">ADC Sample Rate (in Hz): </div>
      <input type="number" min="1" step="1" max="100" id="sampleRate" name="sampleRateInput">
      <button type="button" class="btn" id="setSampleRate" onclick="setSampleRate()">Set</button>
    <br>
    

  </main>

  
  </body>


  <script type = "text/javascript">
  
    function createXmlHttpObject() {
      return new XMLHttpRequest();
    }

    var xmlHttp = createXmlHttpObject();

    function updateValue(elementId, xmlResponse, xmlTag) {
      var element = document.getElementById(elementId);
      var value = xmlResponse.getElementsByTagName(xmlTag)[0].childNodes[0].nodeValue;
      element.innerHTML = value;
    }

    function updateButtonState(buttonId, xmlResponse, xmlTag) {
        var button = document.getElementById(buttonId);
        if (!button) {
            console.error(`Button with id "${buttonId}" not found`);
            return;
        }

        var stateElement = xmlResponse.getElementsByTagName(xmlTag)[0];
        if (!stateElement || !stateElement.childNodes[0]) {
            console.error(`XML tag "${xmlTag}" not found or empty`);
            return;
        }

        var state = stateElement.childNodes[0].nodeValue;
        button.innerHTML = state === "1" ? "Turn OFF" : "Turn ON";

        // Disable the opposite button
        var oppositeButtonId = buttonId === "btn1" ? "btnCharge" : "btn1";
        var oppositeButton = document.getElementById(oppositeButtonId);
        if (oppositeButton) {
            oppositeButton.disabled = (state === "1");
        }
    }

    function updateSohButtonState(xmlResponse) {
        var sohStatus = xmlResponse.getElementsByTagName("SOHSTATUS")[0].childNodes[0].nodeValue;
        var btnSoh = document.getElementById("btnSoh");
        if (sohStatus === "1") {
            btnSoh.innerHTML = "Measuring...";
            btnSoh.disabled = true;
            btnCharge.disabled = true;
            btn1.disabled = true;
        } else {
            btnSoh.innerHTML = "Start";
            btnSoh.disabled = false;
            btnCharge.disabled = false;
            btn1.disabled = false;
        }
    }

    function updateDateTime() {
        var dt = new Date();
        document.getElementById("time").innerHTML = dt.toLocaleTimeString();
        document.getElementById("date").innerHTML = dt.toLocaleDateString();
    }

    function updateBatteryStatus(xmlResponse) {
        var batteryStatus = xmlResponse.getElementsByTagName("BATTERYCONNECTED")[0].childNodes[0].nodeValue;
        var statusElement = document.getElementById("batteryStatus");
        if (batteryStatus === "1") {
            statusElement.innerHTML = "Battery Connected";
            statusElement.style.color = "green";
        } else {
            statusElement.innerHTML = "Battery Not Connected";
            statusElement.style.color = "red";
        }
    }

    function process() {
        if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
            xmlHttp.open("PUT", "xml", true);
            xmlHttp.onreadystatechange = function() {
                if (xmlHttp.readyState == 4) {
                    if (xmlHttp.status == 200) {
                        try {
                            var xmlResponse = xmlHttp.responseXML;
                            if (!xmlResponse) {
                                throw new Error("Invalid XML response");
                            }

                            // Update voltage and current
                            updateValue("v0", xmlResponse, "V0");
                            updateValue("v1", xmlResponse, "V1");

                            // Update SOH measurement results
                            updateValue("r_o", xmlResponse, "R_O");
                            updateValue("soc", xmlResponse, "SOC");

                            // Update button states
                            updateButtonState("btn1", xmlResponse, "DISCHARGESWITCH");
                            updateButtonState("btnCharge", xmlResponse, "CHARGESWITCH");
                            updateSohButtonState(xmlResponse);

                            // Update battery status
                            updateBatteryStatus(xmlResponse);


                        } catch (e) {
                            console.error("Error processing XML response:", e);
                        }
                    } else {
                        console.error("HTTP error:", xmlHttp.status);
                    }
                }
            };
            xmlHttp.send(null);
        }
        setTimeout(process, 100);
    }

    function setDischargePeriod() {
        var xhttp = new XMLHttpRequest(); 
        var dischargePeriod = document.getElementById("dischargePeriod").value;
        xhttp.open("PUT", "/setDischargePeriod?dischargePeriod=" + dischargePeriod, false);
        xhttp.send();
    }

    function setNumDischargeCycles() {
        var xhttp = new XMLHttpRequest(); 
        var numDischargeCycles = document.getElementById("numDischargeCycles").value;
        xhttp.open("PUT", "/setNumDischargeCycles?numDischargeCycles=" + numDischargeCycles, true);
        xhttp.send();
    }

    function setSampleRate() {
        var xhttp = new XMLHttpRequest(); 
        var sampleRate = document.getElementById("sampleRate").value;
        xhttp.open("PUT", "/setSampleRate?sampleRate=" + sampleRate, true);
        xhttp.send();
    }

    function setNumCycles() {
        var xhttp = new XMLHttpRequest(); 
        var numCycles = document.getElementById("numCycles").value;
        xhttp.open("PUT", "/setNumCycles?numCycles=" + numCycles, true);
        xhttp.send();
    }

    function setChargePeriod() {
        var xhttp = new XMLHttpRequest(); 
        var chargePeriod = document.getElementById("chargePeriod").value;
        xhttp.open("PUT", "/setChargePeriod?chargePeriod=" + chargePeriod, true);
        xhttp.send();
    }

    function startSohMeasurement() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("PUT", "startSohMeasurement", true);
        xhttp.send();
    }

    function toggleDischargeBattery() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("PUT", "toggleDischargeBattery", true);
        xhttp.send();
    }

    function toggleChargeBattery() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("PUT", "toggleChargeBattery", true);
        xhttp.send();
    }
        
  </script>

</html>

)=====";
