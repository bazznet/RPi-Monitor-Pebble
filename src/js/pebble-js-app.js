var load = 0;
var host = JSON.parse(localStorage.getItem('host')) || [];
var user = JSON.parse(localStorage.getItem('user')) || [];
var pass = JSON.parse(localStorage.getItem('pass')) || [];
var auth = JSON.parse(localStorage.getItem('auth')) || [];

var Base64 = {

// private property
_keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",

// public method for encoding
encode : function (input) {
    var output = "";
    var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
    var i = 0;

    input = Base64._utf8_encode(input);

    while (i < input.length) {

        chr1 = input.charCodeAt(i++);
        chr2 = input.charCodeAt(i++);
        chr3 = input.charCodeAt(i++);

        enc1 = chr1 >> 2;
        enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
        enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
        enc4 = chr3 & 63;

        if (isNaN(chr2)) {
            enc3 = enc4 = 64;
        } else if (isNaN(chr3)) {
            enc4 = 64;
        }

        output = output +
        this._keyStr.charAt(enc1) + this._keyStr.charAt(enc2) +
        this._keyStr.charAt(enc3) + this._keyStr.charAt(enc4);

    }

    return output;
},

// private method for UTF-8 encoding
_utf8_encode : function (string) {
    string = string.replace(/\r\n/g,"\n");
    var utftext = "";

    for (var n = 0; n < string.length; n++) {

        var c = string.charCodeAt(n);

        if (c < 128) {
            utftext += String.fromCharCode(c);
        }
        else if((c > 127) && (c < 2048)) {
            utftext += String.fromCharCode((c >> 6) | 192);
            utftext += String.fromCharCode((c & 63) | 128);
        }
        else {
            utftext += String.fromCharCode((c >> 12) | 224);
            utftext += String.fromCharCode(((c >> 6) & 63) | 128);
            utftext += String.fromCharCode((c & 63) | 128);
        }

    }

    return utftext;
}

}

function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	if (auth == "1"){
		req.setRequestHeader("Authorization", "Basic " + Base64.encode(user + ":" + pass)); 
	}
	req.send(null);
	if (req.status == 200) {
		load = 1;
		return req.responseText;
	}
}

function Pad(n){
  return n<10 ? '0'+n : n;
}

function Plural(n){
  return n>1 ? 's ' : ' ';
}

function Uptime(value){
  var uptimetext='';
  var years = Math.floor(value / 31556926);
  var rest = value % 31556926;
  var days = Math.floor( rest / 86400);
  rest = value % 86400;
  var hours = Math.floor(rest / 3600);
  rest = value % 3600;
  var minutes = Math.floor(rest / 60);
  var seconds = Math.floor(rest % 60);
  if ( years !== 0 ) { uptimetext += uptimetext + years + " year" + Plural(years); }
  if ( ( years !== 0 ) || ( days !== 0) ) { uptimetext += days + " day" + Plural(days);}
  if ( ( days !== 0 ) || ( hours !== 0) ) { uptimetext += Pad(hours) + " hour" + Plural(hours);}
  uptimetext += Pad(minutes) + " minute" + Plural(minutes);
  uptimetext += Pad(seconds) + " second" + Plural(seconds);
  return uptimetext;
}

function KMG(value, initPre){
  var unit = 1024;
  var prefix = "kMGTPE";
  if (initPre){
    value *= Math.pow(unit,prefix.indexOf(initPre)+1);
  }
  try {
    if (Math.abs(value) < unit) { return value + "B"; }
    var exp = Math.floor(Math.log(Math.abs(value)) / Math.log(unit));
    var pre = prefix.charAt(exp-1);
    return (value / Math.pow(unit, exp)).toFixed(2) + pre + "B";
  }
  catch (e) {
    return "Error";
  }
}

function Percent(value,total){
  return (100*value/total).toFixed(2);
}

var getData = function() {
	var response = HTTPGET(host + "/dynamic.json");
	var dynamic_json = JSON.parse(response);

	response = HTTPGET(host + "/static.json");
	var static_json = JSON.parse(response);

	
	var loads = "Loads: " + dynamic_json.load1 + " - " + dynamic_json.load5 + " - " + dynamic_json.load15;
	var cpu_frecuency = "CPU frequency: " + dynamic_json.cpu_frequency + "MHz";
	var cpu_voltage = "Voltage: " + dynamic_json.cpu_voltage + "V";
	var cpu_temperature = "CPU Temperature: " + dynamic_json.soc_temp + "\u00B0C";	
	var uptime = "Uptime: " + Uptime(dynamic_json.uptime);
	var cpu = loads + "\n" + cpu_frecuency + "\n" + cpu_voltage + "\n" + cpu_temperature + "\n" + uptime;

	var memory_total = "Total: " + KMG(static_json.memory_total, "M");
	var memory_available = "Available: " + KMG(dynamic_json.memory_available, "M");
	var memory_percent = Percent((static_json.memory_total-dynamic_json.memory_available), static_json.memory_total) ;
	var memory_used = "Used: " + KMG((static_json.memory_total-dynamic_json.memory_available), "M") + " (" + memory_percent + "%)";
	var memory = memory_used + "\n" + memory_available + "\n" + memory_total;

	var swap_free = "Free: " + KMG((static_json.swap_total-dynamic_json.swap_used), "M");
	var swap_percent = (Percent(dynamic_json.swap_used, static_json.swap_total) == "NaN") ? 0 : Percent(dynamic_json.swap_used, static_json.swap_total);
	var swap_used = "Used: " + KMG(dynamic_json.swap_used, "M") + " (" + swap_percent + "%)";
	var swap_total = "Total: " + KMG(static_json.swap_total, "M");
	var swap = swap_used + "\n" + swap_free + "\n" + swap_total;

	var sdcard_total = "Total: " + KMG(static_json.sdcard_root_total, "M");
	var sdcard_percent = Percent(dynamic_json.sdcard_root_used, static_json.sdcard_root_total);
	var sdcard_used = "Used: " + KMG(dynamic_json.sdcard_root_used, "M") + " (" + sdcard_percent + "%)";
	var sdcrad_free = "Free: " + KMG((static_json.sdcard_root_total-dynamic_json.sdcard_root_used), "M");
	var sdcard = sdcard_used + "\n" + sdcrad_free + "\n" + sdcard_total;

	var net_send = "Ethernet Sent: " + KMG(dynamic_json.net_send);
	var net_received = "Received: " + KMG(Math.abs(dynamic_json.net_received));
	var net = net_send + "\n" + net_received;

	/*
	var processor = static_json.processor;
	var distribution = static_json.distribution;
	var kernel_version = static_json.kernel_version;
	var firmware = "Firmware: " + static_json.firmware;
	var upgrade = "Package(s): " + dynamic_json.upgrade;
	var version = processor + "\n" + distribution + "\n" + kernel_version + "\n" + firmware + "\n" + upgrade;
	*/
	var TempLaval = "Laval: " +dynamic_json.TempLaval;
	var HumBase = "Basement Hum: " +dynamic_json.Humidity;
	var TempBase = "Basement Temp: " +dynamic_json.Temp;
	var TempCar = "Car Temp: " +dynamic_json.TempCar;
	var TempSalon = "Salon Temp: " +dynamic_json.TempSalon;
	var TempEntree = "Entree Temp: " +dynamic_json.TempEntree;
	var version = TempLaval + "\n" + HumBase + "\n" + TempBase + "\n" + TempCar + "\n" + TempSalon + "\n" + TempEntree;
	
	
	
	var dict = {"OK" : load, "CPU" : cpu, "MEMORY" : memory, "MEMORY_P" : Math.round(memory_percent), "SWAP" : swap, "SWAP_P" : Math.round(swap_percent), "SDCARD" : sdcard, "SDCARD_P" : Math.round(sdcard_percent), "NET" : net, "HOME" : version};
	
	Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("showConfiguration",
  function(e) {
	Pebble.openURL("http://laubaz.no-ip.org/pebble.html");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    var options = JSON.parse(decodeURIComponent(e.response));
    localStorage.setItem('host', JSON.stringify(options.host));
	localStorage.setItem('user', JSON.stringify(options.user));
	localStorage.setItem('pass', JSON.stringify(options.pass));
	localStorage.setItem('auth', JSON.stringify(options.auth));
  }
);

Pebble.addEventListener("ready", function(e) {
	"use strict";
	if (host) {
		//console.log(host);
		getData();
	}	
});
