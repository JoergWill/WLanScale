# include <stdlib.h>

String pageContent;
ESP8266WebServer webserver;


void setupWeb() {
	webserver = ESP8266WebServer(8080);
	webserver.on("/", handleIndexPage);
	webserver.on("/xml", handleXML);
	webserver.on("/params", handleParams);

	webserver.begin();
}

void loopWeb() {
	webserver.handleClient();
}

void handleParams() {
	Serial.println("Handle Params");
	Serial.println();

	if (webserver.args() > 0) {
		for (uint8_t i = 0; i < webserver.args(); i++) {
			Serial.print(webserver.argName(i));
			Serial.print(" ");
			Serial.println(webserver.arg(i));
		}
	}
	Serial.println();
}

void handleXML() {
	char Gewicht[10];

	//Serial.print("XML-Abfrage");
	pageContent = "<?xml version='1.0' encoding='UTF-8'?>";
	pageContent += "<winback>";
	pageContent += "<scale category='ad104'>";

	// format weight in [kg]
	dtostrf(weight/1000, 10, 3, Gewicht);
	pageContent += "<weight lang='de'>";
	for (uint8_t i = 0; i<sizeof(Gewicht); i++) pageContent += Gewicht[i];
	pageContent += " kg</weight>";

	// state serial connection
	if (ad104Offline) 
		pageContent += "<ad104>&#10060; AD104</ad104>";
	else 
		pageContent += "<ad104>&#9989; AD104</ad104>";
	
	// state winback-connect
	if (winbackOffline)
		pageContent += "<wbcon>&#10060; WinBack</wbcon>";
	else
		pageContent += "<wbcon>&#9989; WinBack</wbcon>";

	// accu
	//pageContent += "<accu>" + String(14) + "</accu>";

	pageContent += "</scale>";
	pageContent += "</winback>";
	webserver.send(200, "text/xml", pageContent);
}

void handleIndexPage(void) {
	//Serial.print("Web-Abfrage");

	pageContent = "<!DOCTYPE html>";
	pageContent += "<html lang='de'>";
	pageContent += "<head>";
	pageContent += "<meta charset='UTF-8' />";
	pageContent += "<title>WinBack</title>";
	pageContent += "<meta name = 'viewport' content = 'width=device-width, initial-scale=1'>";

	pageContent += "<style>";
	pageContent += "*{box-sizing:border-box;}";
	pageContent += "body{font-family:Arial, Helvetica, sans-serif; margin:0;}";

	/* Style the header */
	pageContent += ".header{padding:1px; text-align:center; background:#1abc9c; color:white;}";
	/* Increase the font size of the h1 element */
	pageContent += ".header h1{font-size:80px;}";
	/* Footer */
	pageContent += ".footer{padding:1px; padding-left:10px; text-align:left ;background:#ddd; margin-top:0px; font-size:10px;}";

	/* Left column */
	pageContent += ".leftcolumn{float:left; width:75%;}";
	/* Right column */
	pageContent += ".rightcolumn{float:right; width:25%; padding-left:20px;}";
	/* Clear floats after the columns */
	pageContent += ".row:after{content:''; display:table; clear:both;}";

	/* Style the top navigation bar */
	pageContent += ".navbar{overflow:hidden; background-color:#333;}";
	/* Style the navigation bar links */
	pageContent += ".navbar a{float:left; display:block; color:white; text-align:center; padding:14px 20px; text-decoration:none;}";
	/* Right-aligned link */
	pageContent += ".navbar a.right{float:right;}";
	/* Change color on hover */
	pageContent += ".navbar a:hover{background-color:#ddd; color:black;}";

	/* The Modal(background) */
	/* Hidden by default, Stay in place, Sit on top, Full width, Full Height, Enable scroll if needed */
	pageContent += ".modal{display:none; position:fixed; z-index:1;	left:0; top:0; width:100 %; height:100 %; overflow:auto; background-color:rgb(0,0,0); background-color:rgba(0,0,0,0.4); padding-top:60px;}";
	/* Modal Content/Box, 5% from the top, 15% from the bottom and centered */
	pageContent += ".modal-content{background-color:#ddd; margin:5% auto 15% auto; border:1px solid #888; width:80%;}";
	/* Add Zoom Animation */
	pageContent += ".animate{-webkit-animation:animatezoom 0.6s; animation:animatezoom 0.6s}";
	pageContent += "@-webkit-keyframes animatezoom{from{-webkit-transform:scale(0)}to{-webkit-transform:scale(1)}}";
	pageContent += "@keyframes animatezoom{from{transform:scale(0)}to{transform:scale(1)}}";
	/* Set a style for all buttons */
	pageContent += "button{background-color:#4CAF50; color:white; padding:14px 20px; margin:8px 8px; border:none; cursor:pointer; width:20 %;}";
	pageContent += "button:hover{opacity:0.8;}";
	/* Extra styles for the cancel button */
	pageContent += ".cancelbtn{width:20 %; padding:10px 20;background-color:#f44336;}";
	
	pageContent += "</style>";
	pageContent += "</head>";

	/* Headline */
	pageContent += "<div class='footer'>";
	pageContent += "<p style='font-size:16px;'>WLan Waage</p>";
	pageContent += "</div>";

	/* Create two unequal columns that floats next to each other */
	pageContent += "<div class='row'>";
	pageContent += "	<div class='leftcolumn'>";
	pageContent += "		<div class='header'>";
	pageContent += "			<h1 id='weight'></h1>";
	pageContent += "		</div>";
	pageContent += "	</div>";

	pageContent += "	<div class='rightcolumn'>";
	pageContent += "		<p>Status Verbindung:</p>";
	pageContent += "		<p id='ad104'></p>";
	pageContent += "		<p id='wbcon'></p>";

	/* Symbol Accu*/
	//pageContent += "<svg width='200' height='20'>";
	//pageContent += "<image width='14' height='20' xlink href='data:image/gif;base64,R0lGODlhHAApAPcAADExMTk5OUpKSmNjY3Nzc4yMjJycnOfn5/f39////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ywAAAAAHAApAAAIuQATCBxYQACAgwgTHhRgYKBDhwUUSlTY8KHDAAAwZjyoseNCiwcIGNQ4sSQAAQQOJDhAEmFLkjATBggJYIBKizhBDgBAIMDMnEAfHjh5MKjRgQmPHk2qNCjTpjmfQrUodarDqlYFYs261WrXqV+hhm06VmnZpQizUk2r9irbtlrfwj1rlK5TuW3tAtUbFa9avjgBry0KNy7hwoIfJnbrEwECuENR1ryZ9cDOlD5NapapMqTBzSVRqgwIADs=' />";
	//pageContent += "<rect id='accu' x=2 y=4 width=10 height=14 style='fill:rgb(0,255,0);'/>";
	//pageContent += "<text fill='black' font-size=16 font-family='Arial' x=20 y=18>Akku 100%</text>";
	//pageContent += "</svg>";

	pageContent += "	</div>";
	pageContent += "</div>";

	pageContent += "<div class = 'navbar'>";
	pageContent += "<a href = '#'>Tara</a>";
	pageContent += "<a onclick=\"document.getElementById('adj104').style.display='block'\">Scale</a>";
	pageContent += "<a href = '#'>Admin</a>";
	pageContent += "<a href = '#' class = 'right'>Home</a>";
	pageContent += "</div>";

	/* Footer */
	pageContent += "<div class = 'footer'>";
	pageContent += "<p>(c) 2019 WinBack GmbH</p>";
	pageContent += "</div>";

	/* Modal Form adjust scale */
	pageContent += "<div id='adj104' class='modal'>";
	pageContent += "	<form class='modal-content animate'>";
	pageContent += "	<div class='container'>";
	pageContent += "		<p style='font-size:20px; text-align:center;'>Abgleich Waage (Nullpunkt und Verst&aumlrkung)</p>";
	pageContent += "	</div>";
	pageContent += "	<div class='container'>";
	pageContent += "		<label for='maxgew'>Maximalgewicht</label>";
	pageContent += "		<input id='m1' type='text' name='maxgew'>";
	pageContent += "	</div>";
	pageContent += "	<div class='container'>";
	pageContent += "		<label for='abglgew'>Abgleichgewicht</label>";
	pageContent += "		<input id='m2' type='text' name='abglgew'>";
	pageContent += "	</div>";
	pageContent += "	<div class='container'>";
	pageContent += "		<button type='button' onclick=\"SendParams('NULL')\" >Nullpunkt</button>";
	pageContent += "		<button type='button' onclick=\"SendParams('SPAN')\" >Span</button>";
	pageContent += "		<button type='button' onclick=\"document.getElementById('adj104').style.display='none'\" class='cancelbtn'>Abbrechen</button>";
	pageContent += "	</div>";
	pageContent += "	</form>";
	pageContent += "</div>";


	/* Javascript */
	pageContent += "<script>";
	pageContent += "var myVar = setInterval(myTimer, 1000);";
	pageContent += "function myTimer() {";
	pageContent += " var xhttp = new XMLHttpRequest();";
	pageContent += " xhttp.onreadystatechange = function() {";
	pageContent += "  if (this.readyState == 4 && this.status == 200) {";
	pageContent += "   var xmlDoc = this.responseXML;";
	// weight in [kg]
	pageContent += "   var x = xmlDoc.getElementsByTagName('weight')[0];";
	pageContent += "   var w = x.childNodes[0];";
	pageContent += "   document.getElementById('weight').innerHTML = w.nodeValue;";
	// state serial connection ad104
	pageContent += "   var x = xmlDoc.getElementsByTagName('ad104')[0];";
	pageContent += "   var w = x.childNodes[0];";
	pageContent += "   document.getElementById('ad104').innerHTML = w.nodeValue;";
	// state winback-ip-connection
	pageContent += "   var x = xmlDoc.getElementsByTagName('wbcon')[0];";
	pageContent += "   var w = x.childNodes[0];";
	pageContent += "   document.getElementById('wbcon').innerHTML = w.nodeValue;";
	// state accu
	//pageContent += "   var x = xmlDoc.getElementsByTagName(\"accu\")[0];";
	//pageContent += "   var w = x.childNodes[0];";
	//pageContent += "   var v = w.nodeValue;";
	//pageContent += "   document.getElementById('accu').setAttribute('y', 3 + v);";
	//pageContent += "   document.getElementById('accu').setAttribute('height', v);";

	pageContent += "  }";
	pageContent += " };";
	pageContent += "xhttp.open('GET','xml',true);";
	pageContent += "xhttp.send();";
	pageContent += "};";

	// Send Parameter Abgleich Waage
	pageContent += "function SendParams(func) {";
	pageContent += " var xhr = new XMLHttpRequest();";
	pageContent += " 	xhr.open('POST', 'params', true);";
	pageContent += " 	xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
	pageContent += " 	xhr.onload = function() {console.log(this.responseText);};";
	pageContent += "	var p1 = 'maxgew=';";
	pageContent += "	var m1 = document.querySelector('#m1').value;";
	pageContent += "	var p2 = '&abglgew=';";
	pageContent += "	var m2 = document.querySelector('#m2').value;";
	pageContent += "	var p3 = '&func=';";
	pageContent += " 	xhr.send(p1.concat(m1).concat(p2).concat(m2).concat(p3).concat(func));";
	pageContent += "};";

	// When the user clicks anywhere outside of the modal, close it
	pageContent += "var modal=document.getElementById('adj104');";
	pageContent += "window.onclick=function(event) {if (event.target==modal) {modal.style.display='none';}}";

	pageContent += "</script>";
	pageContent += "</body>";
	pageContent += "</html>";

	webserver.send(200, "text/html", pageContent);
}
