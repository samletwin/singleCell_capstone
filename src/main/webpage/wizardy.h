/*


  OK, ya ready for some fun? HTML + CSS styling + javascript all in and undebuggable environment

  one trick I've learned to how to debug HTML and CSS code.

  get all your HTML code (from html to /html) and past it into this test site
  muck with the HTML and CSS code until it's what you want
  https://www.w3schools.com/html/tryit.asp?filename=tryhtml_intro

  No clue how to debug javascrip other that write, compile, upload, refresh, guess, repeat

  I'm using class designators to set styles and id's for data updating
  for example:
  the CSS class .tabledata defines with the cell will look like
  <td><div class="tabledata" id = "switch"></div></td>

  the XML code will update the data where id = "switch"
  java script then uses getElementById
  document.getElementById("switch").innerHTML="Switch is OFF";


  .. now you can have the class define the look AND the class update the content, but you will then need
  a class for every data field that must be updated, here's what that will look like
  <td><div class="switch"></div></td>

  the XML code will update the data where class = "switch"
  java script then uses getElementsByClassName
  document.getElementsByClassName("switch")[0].style.color=text_color;


  the main general sections of a web page are the following and used here

  <html>
    <style>
    // dump CSS style stuff in here
    </style>
    <body>
      <header>
      // put header code for cute banners here
      </header>
      <main>
      // the buld of your web page contents
      </main>
      <footer>
      // put cute footer (c) 2021 xyz inc type thing
      </footer>
    </body>
    <script>
    // you java code between these tags
    </script>
  </html>


*/

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

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
    
  </style>

  <body style="background-color: #efefef" onload="process()">
  
    <header>
      <div class="navbar fixed-top">
          <div class="container">
            <div class="navtitle">BMS Capstone Website Control</div>
            <div class="navdata" id = "date">mm/dd/yyyy</div>
            <div class="navheading">DATE</div><br>
            <div class="navdata" id = "time">00:00:00</div>
            <div class="navheading">TIME</div>
            
          </div>
      </div>
    </header>
  
    <main class="container" style="margin-top:70px">
      <div class="category">Battery Readings</div>
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
    <div class="category">Battery Controls</div>
    <br>
      <div class="bodytext">Start SOH Measurement: </div>
      <button type="button" class = "btn" id = "btnSoh" onclick="startSohMeasurment()">Start</button>
      </div>
      <br>
      <div class="bodytext">Battery Measurement Switch: </div>
      <button type="button" class = "btn" id = "btn1" onclick="toggleCurrentMeasurement()">Toggle</button>
      </div>
      <br>
      <div class="bodytext">Discharge Period (in ms): </div>
      <input type="number" min="1" step="1" max="65535" id="dischargePeriod" name="dischargePeriodInput">
      <button type="button" class="btn" id="setDischargePeriod" onclick="setDischargePeriod()">Set</button>
      <br>
   
      <div class="bodytext">Number of Discharge Cycles: </div>
      <input type="number" min="0" step="1" max="255" id="numDischargeCycles" name="numDischargeCyclesInput">
      <button type="button" class="btn" id="setNumDischargeCycles" onclick="setNumDischargeCycles()">Set</button>
      <br>

      <div class="bodytext">Sample Rate (in Hz): </div>
      <input type="number" min="1" step="1" max="100" id="sampleRate" name="sampleRateInput">
      <button type="button" class="btn" id="setSampleRate" onclick="setSampleRate()">Set</button>
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
        <td><div class="bodytext">OCV (V)</div></td>
        <td><div class="tabledata" id = "ocv"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">Internal Resistance (Ohms)</div></td>
        <td><div class="tabledata" id = "r_o"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">SOC (%)</div></td>
        <td><div class="tabledata" id = "soc"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">TTS (s)</div></td>
        <td><div class="tabledata" id = "tts"></div></td>
      </table>
    </div>

  </main>

  
  </body>


  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function startSohMeasurment() {
      var xhttp = new XMLHttpRequest(); 
      var message;

      
      // Disable the button - don't re-enable until measurement is complete
      // document.getElementById("btnSoh").disabled = true;
      xhttp.open("PUT", "startSohMeasurment", false);
      xhttp.send();

      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      // xhttp.onreadystatechange = function() {
      //   if (this.readyState == 4 && this.status == 200
      //     && this.responseText = "enableBtnSoh") {
      //     document.getElementById("btnSoh").disabled = false;
      //   }
      // }
    }


    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function toggleCurrentMeasurement() {
      var xhttp = new XMLHttpRequest(); 
      xhttp.open("PUT", "toggleCurrentMeasurement", false);
      xhttp.send(); 
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

    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
     
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
      
      //volts for A0
      xmldoc = xmlResponse.getElementsByTagName("V0"); 
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      
      barwidth = message / 40.95;
      document.getElementById("v0").innerHTML=message;
      document.getElementById("v0").style.width=(barwidth+"%");
      // you can set color dynamically, maybe blue below a value, red above
      document.getElementById("v0").style.backgroundColor=color;
      //document.getElementById("v0").style.borderRadius="5px";
  
      // Volts1    
      xmldoc = xmlResponse.getElementsByTagName("V1");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      width = message / 40.95;
      document.getElementById("v1").innerHTML=message;
      document.getElementById("v1").style.width=(width+"%");
      document.getElementById("v1").style.backgroundColor=color;
      //document.getElementById("v1").style.borderRadius="5px";

      // OCV
      xmldoc = xmlResponse.getElementsByTagName("OCV");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      width = message / 40.95;
      document.getElementById("ocv").innerHTML=message;
      document.getElementById("ocv").style.width=(width+"%");
      document.getElementById("ocv").style.backgroundColor=color;
      //document.getElementById("ocv").style.borderRadius="5px";

      // R_O   
      xmldoc = xmlResponse.getElementsByTagName("R_O");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      width = message / 40.95;
      document.getElementById("r_o").innerHTML=message;
      document.getElementById("r_o").style.width=(width+"%");
      document.getElementById("r_o").style.backgroundColor=color;
      //document.getElementById("r_o").style.borderRadius="5px";

      // SOC 
      xmldoc = xmlResponse.getElementsByTagName("SOC");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      width = message / 40.95;
      document.getElementById("soc").innerHTML=message;
      document.getElementById("soc").style.width=(width+"%");
      document.getElementById("soc").style.backgroundColor=color;
      //document.getElementById("soc").style.borderRadius="5px";
      
      // TTS    
      xmldoc = xmlResponse.getElementsByTagName("TTS");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 2048){
        color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      width = message / 40.95;
      document.getElementById("tts").innerHTML=message;
      document.getElementById("tts").style.width=(width+"%");
      document.getElementById("tts").style.backgroundColor=color;
      //document.getElementById("tts").style.borderRadius="5px";

      // xmldoc = xmlResponse.getElementsByTagName("SOH");
      // message = xmldoc[0].firstChild.nodeValue;
  
      // if (message == 0){
      //   document.getElementById("btnSoh").innerHTML="Measuring...";
      // }
      // else{
      //   document.getElementById("btnSoh").innerHTML="Start";
      // }
         
      xmldoc = xmlResponse.getElementsByTagName("SWITCH");
      message = xmldoc[0].firstChild.nodeValue;
      // update the text in the table
      if (message == 0){
        document.getElementById("btn1").innerHTML="Turn ON";
      }
      else {
        document.getElementById("btn1").innerHTML="Turn OFF";
      }
     }
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",100);
    }

    function responseSoh() {
      xmldoc = xmlResponse.getElementsByTagName("SOH");
      message = xmldoc[0].firstChild.nodeValue;
  
      if (message == 0){
        document.getElementById("btnSoh").innerHTML="Measuring...";
      }
      else{
        document.getElementById("btnSoh").innerHTML="Start";
      }
    }
    function processSohButton(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xmlSoh",true);
        xmlHttp.onreadystatechange=responseSoh;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("processSohButton()",100);
    }
  
  
  </script>

</html>



)=====";