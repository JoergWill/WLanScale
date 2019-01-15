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
	webserver.send(200, "text/xml", String(weight));
}

void handleIndexPage(void) {
	Serial.print("Web-Abfrage");

	pageContent = "<!DOCTYPE html>";
	pageContent += "<html lang=\"de\">";
	pageContent += "<head>";
	pageContent += "<meta charset=\"UTF-8\" />";
	pageContent += "<title>WinBack</title>";

	pageContent += "<meta name = \"viewport\" content = \"width=device-width, initial-scale=1\">";
	pageContent += "<style>";
	pageContent += "body{font-family: Arial, Helvetica, sans-serif;margin:0;}";

	/* Style the header */
	pageContent += ".header{padding: 80px;text-align: center;background: #1abc9c;color: white;}";
	/* Increase the font size of the h1 element */
	pageContent += ".header h1{font-size: 40px;}";

	/* Style the top navigation bar */
	pageContent += ".navbar{overflow: hidden;background-color: #333;}";
	/* Style the navigation bar links */
	pageContent += ".navbar a{float: left;display: block;color: white;text-align: center;padding: 14px 20px;text-decoration: none;}";
	/* Right-aligned link */
	pageContent += ".navbar a.right{float: right;}";
	/* Change color on hover */
	pageContent += ".navbar a:hover{background-color: #ddd;color: black;}";

	pageContent += "</style>";
	pageContent += "</head>";

	pageContent += "<body>";
	pageContent += "<div class = \"header\">";
	pageContent += "<h1 id=\"weight\"></h1>";
	pageContent += "<p>WLan-Waage</p>";
	pageContent += "</div>";

	pageContent += "<div class = \"navbar\">";
	pageContent += "<a href = \"#\">Tara</a>";
	pageContent += "<a href = \"#\">Scale</a>";
	pageContent += "<a href = \"#\">Admin</a>";
	pageContent += "<a href = \"#\" class = \"right\">Home</a>";
	pageContent += "</div>";

	pageContent += "<script>";
	pageContent += "var myVar = setInterval(myTimer, 1000);";
	pageContent += "function myTimer() {";
	pageContent += "var xhttp = new XMLHttpRequest();";
	pageContent += "xhttp.onreadystatechange = function() {";
	pageContent += "if (this.readyState == 4 && this.status == 200) {";
	pageContent += "document.getElementById(\"weight\").innerHTML = this.responseText;";
	pageContent += "}";
	pageContent += "};";
	pageContent += "xhttp.open(\"GET\", \"xml\", true);";
	pageContent += "xhttp.send();";
	pageContent += "}";
	pageContent += "</script>";

	pageContent += "</body>";
	pageContent += "</html>";
	webserver.send(200, "text/html", pageContent);
}

void printweight() {
	pageContent += "Gewicht " + String(weight) + " kg" + ".<br>\n";
}
