String pageContent;
ESP8266WebServer webserver;
//void handleIndexPage(void);

void setupWeb() {
	webserver = ESP8266WebServer(8080);
	webserver.on("/", handleIndexPage);
	webserver.on("/xml", handleXML);

	webserver.begin();
}

void loopWeb() {
	webserver.handleClient();
}

void handleXML() {
	Serial.print("XML-Abfrage");
	webserver.send(200, "text/xml", String(weight) + " kg");
}

void handleIndexPage(void) {
	Serial.print("Web-Abfrage");

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
	pageContent += ".footer{padding:1px; text-align:left ;background:#ddd; margin-top:0px; font-size:10px;}";

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

	pageContent += "</style>";
	pageContent += "</head>";

	/* Headline */
	pageContent += "<div class='footer'>";
	pageContent += "<p style='font-size:16px;'>WLan Waage</p>";
	pageContent += "</div>";

	/* Create two unequal columns that floats next to each other */
	pageContent += "<div class='row'>";
	pageContent += "<div class='leftcolumn'>";
	pageContent += "<div class='header'>";
	pageContent += "<h1 id='weight'></h1>";
	pageContent += "</div>";
	pageContent += "</div>";

	pageContent += "<div class='rightcolumn'>";
	pageContent += "<p>TEST</p>";
	pageContent += "<p>TEST</p>";
	pageContent += "</div>";
	pageContent += "</div>";

	pageContent += "<div class = 'navbar'>";
	pageContent += "<a href = '#'>Tara</a>";
	pageContent += "<a href = '#'>Scale</a>";
	pageContent += "<a href = '#'>Admin</a>";
	pageContent += "<a href = '#' class = 'right'>Home</a>";
	pageContent += "</div>";

	/* Footer */
	pageContent += "<div class = 'footer'>";
	pageContent += "<p>(c) 2019 WinBack GmbH</p>";
	pageContent += "</div>";

	/* Javascript */
	pageContent += "<script>";
	pageContent += "var myVar = setInterval(myTimer, 1000);";
	pageContent += "function myTimer() {";
	pageContent += " var xhttp = new XMLHttpRequest();";
	pageContent += " xhttp.onreadystatechange = function() {";
	pageContent += "  if (this.readyState == 4 && this.status == 200) {";
	pageContent += "   document.getElementById('weight').innerHTML = this.responseText;";
	pageContent += "  }";
	pageContent += " };";
	pageContent += "xhttp.open('GET', 'xml', true);";
	pageContent += "xhttp.send();";
	pageContent += "}";
	pageContent += "</script>";

	pageContent += "</body>";
	pageContent += "</html>";
	webserver.send(200, "text/html", pageContent);
}
