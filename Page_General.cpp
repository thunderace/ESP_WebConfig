#include "global.h"

const char PAGE_AdminGeneralSettings[] =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value="" maxlength="20"></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html() {
	String values = "devicename|" + (String) config.deviceName + "|div\n";
	server.send ( 200, "text/plain", values);
	//Serial.println(__FUNCTION__); 
}

void send_general_html() {
	if (server.args() > 0) { // Save Settings
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") copyConfigString(config.deviceName, urldecode(server.arg(i)).c_str(), DEVICE_NAME_MAX_SIZE); 
		}
		writeConfig();
	}
	server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	//Serial.println(__FUNCTION__); 
}

void send_general_configuration_values_html() {
	String values = "devicename|" +  (String)  config.deviceName +  "|input\n";
	server.send ( 200, "text/plain", values);
	//Serial.println(__FUNCTION__); 
}
