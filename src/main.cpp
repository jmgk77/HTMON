//HTMON
//(c) JMGK 2021

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

//#define DEBUG

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include <SSD1306Wire.h>
#include <WEMOS_SHT3X.h>

#include "logo.h"

#define READ_INTERVAL 1

#define RELAY_PIN D3

SSD1306Wire display(0x3c, SDA, SCL);
SHT3X sht30(0x44);

WiFiManager wm;
ESP8266WebServer server;
ESP8266HTTPUpdateServer httpUpdater;

time_t base_time;

float t, h;
unsigned long lastMillis;

#define HT_SIZE 720
#define HT_REWIND 60
unsigned int th_index;
float t_table[HT_SIZE];
float h_table[HT_SIZE];

int light_state = 0;

const char html_header[] PROGMEM = R""""(<!DOCTYPE html>
<html lang='pt-br'>
<head>
<meta http-equiv='refresh' content='60'>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<meta http-equiv='cache-control' content='no-cache, no-store, must-revalidate'>
<title>HTMON</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<link rel='shortcut icon' href=')"""";

const char html_header2[] PROGMEM = R""""('/>
<style>
.deg_btn{position:relative;display:inline-block;text-decoration:none;padding:0 30px;font-size:19px;height:40px;line-height:40px!;vertical-align:middle;background:#51587b;font-size:20px;color:#fff;transition:.4s}.deg_btn:before{position:absolute;content:'';left:0;top:0;width:0;height:0;border:none;border-left:solid 21px #fff;border-bottom:solid 41px transparent;z-index:1;transition:.4s}.deg_btn:after{position:absolute;content:'';right:0;top:0;width:0;height:0;border:none;border-left:solid 21px transparent;border-bottom:solid 41px #fff;z-index:1;transition:.4s}.deg_btn:hover:after,.deg_btn:hover:before{border-left-width:25px}.deg_btn:hover{background:#2c3148}#outer{width:100%;text-align:center}.inner{display:inline-block}a
</style>
</head>
<body>)"""";

const char light_on[] PROGMEM = R""""(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAABrVBMVEUAAABjYgRVVAUAAC0AAF5+fAlycAkWFgxHSAAAAArMvETKukbKukdgYARGRQOulm8AAAIKCgH//wAZGAQODRHW0teNiwgNDAMMDAAfHhLFwclUUjxhX0kTETYCAgAvLyAyMQFBPxgHCAAoJxlQUAMlJQcjIhJzdgAfHwEREQQSEgkfHgggHhRGRgAmJQcoJhKfngUUEwAODgQXFg4DAgkHBRApKAUhIAwrKB6GhQYaGQIPDwEWFgYVFBAqKQNeXQB5eABnZwBRUAAdHQIZFxwyMQAjIgUjIgu1sLyHhgcPDwQNDAQZGA01NQCSkgBpaQAqKQMkJAgdHA0iIRO/usQNDAN3dgBbWgMSExgTEgGgoABvbgMlJhcIBwGEhAB+fQQbGh0AAAM6OgC1tQCysgFIRwsNCz8AAABEQwBMTAsWFSsaGgGBgAC2tgB+fgIgHxQiIgQlJRQqKhojIhUdHBIcHAQlJRQsLB4jIhgnJRxPTgAdHRArKyMhIBmDfmIAAADk5ADj4wCjowCGhQD09AD//wD9/QDd3QCHhwC6ugD7+wD8/ADW1gDS0gAAAADnl2gqAAAAgHRSTlMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACMCwBHEoETEcCRSQBB1sgGhkbbA0BNzcIBj6SvbuLNggJPTYBARpRI1Dk3UQoWRsBF87EETDv6yoc2dYZAmz09WwCBoaHBznl8OQ2L8XRwysZrt+pGAJjnmIBAVb1qlYAAAABYktHRACIBR1IAAAAB3RJTUUH5QwPFRIwUw78IgAAANRJREFUGNNjYGBgYGRiYGBmYWBgZWOAABUGdgZVNQYOTnUQj4ubR0NTi1dbh09XT5+fn4HBQEDQ0MhYyMRUyMzcQlhElMHSytrG1s7ewdHJ2cXVzd2DwdPL28fXr6Gxqdk/IDAoOIRBTJwhNKyltbWtvSM8QkJSCmisdGRUKwh0RsfIQKyNjesCCbTEJ0DdkZiU3N3a2p2SmgYVEEjP6Glt7c3MkoUKMMhl5+Tm5uXLM8BBQWFRUXEJgs9QWlZeXlGJJFBVXVNTW4ckoKBYX6+kDGYCAP4LMfPqfXhdAAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDIxLTEyLTE2VDAwOjE4OjQ4LTAzOjAwf0AOeAAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyMS0xMi0xNlQwMDoxODo0OC0wMzowMA4dtsQAAAAASUVORK5CYII=)"""";

const char light_off[] PROGMEM = R""""(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAMAAAAoLQ9TAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAABp1BMVEUAAAA0MzItLCtFQ0BAPjsPDw8hIiABAQOcjHMmJSSikHMFBQX////FxccODg4PDg/e2+FNTEoHBwcBAgAYFxjPzdE+PT1QTlIhHyEBAQEnJycaGhk3NzYLDQofHh4qKSkWFRUaGRktMCYQEBAKCgoMDAwTExIaGBkgIB8WFRYdGxxQT0wKCgkJCAgREBEGBQULCgoWFhYWFRYkISRHRkUODQ0ICAgODQ0SEBEWFhYvLi48PDwzMzMoKCgPDw8ZFxkZGRgUExMXFhbCv8RJSEYJCQkICAgSEhIbGhpISEg0NDQWFRYVFRUUExQaGRrKx8wIBwc7OjouLi4SExMKCglPT084ODgdHR8EBARBQUFAQEAbGhwAAAAdHRxZWVlZWVkpKCkiHyEAAAAiISErKysgICAODg0/Pz9bWlpaWlo/Pz8ZGBkTExIbGxsgICAbGhoXFhYQEA8cHBwlJSQkJCQcHBwhHyApKCcWFhYmJiYmJSUcGxxva2wAAABxcXFxcHBRUVFCQkJ5eXl/f39+fn59fX1tbW1cXFx8fHxqaWmAgIBoaGgAAAAU5oS+AAAAfnRSTlMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAjAsARxKBExHAkUkAQdbIBoZG2wNATc3CAY+kr27izYICT02AQEaUSNQ5N1EKFkbARfOxBEw7+sqHNnWGQJs9PVsAgaGhwc55fDw5DYvxdHDKxmu39+pGAJjnp5iAQHIT7PPAAAAAWJLR0QAiAUdSAAAAAd0SU1FB+UMDxUSCHsMRLwAAADTSURBVBjTY2BgYGBkYgADZhYIzSDPwMqgoMjAxq4E4nFwcCirqDKqqXNqaGpxcTEwaDNw6+jq8egb8BoaGfPxCzCYmJqZW1haWdvY2tk7ODo5M7i4url7eNbVNzR6efv4+vkzCAoxBAQ2Nbe0tjUGBQuLiAKNFQsJbW5taWluDwsXh1gbEdnR0tra0hQVDXVHTGwcUEVrfEIiVIAhKbmzq6s7JVUCJiCZlp6RmZUtxQAHObl5efkFCD5DYVFxSWkZkkB5RWVVdQ2SgLRMba2sHJgJAF1JL018Y5qxAAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDIxLTEyLTE2VDAwOjE4OjA4LTAzOjAw+woAggAAACV0RVh0ZGF0ZTptb2RpZnkAMjAyMS0xMi0xNlQwMDoxODowOC0wMzowMIpXuD4AAAAASUVORK5CYII=)"""";

const char html_footer[] PROGMEM = R""""(
</body>
</html>
)"""";

const char html_main[] = R""""(
<div id='outer'>
<div class='inner'><p class='deg_btn'>HTMON</p></div>
<canvas id='canvas' width='600' height='200'></canvas>
<div class='inner'><a href='/light' class='deg_btn'>DEHUMIDIFY</a>
<a href='/update' class='deg_btn'>UPDATE</a></div>
</div>
)"""";

const char js_script[] = R""""(
var canvas = document.getElementById('canvas');
var ctx = canvas.getContext('2d');
var myChart = new Chart(ctx, {
type: 'line',
data: {
labels: l_data,
datasets: [{
label: 'Temperature',
data: t_data,
borderColor: 'rgb(255, 0, 0)',
backgroundColor: 'rgb(255, 0, 0, 0.1)',
tension: 0.1,
}, {
label: 'Humidity',
data: h_data,
borderColor: 'rgb(0, 0, 255)',
backgroundColor: 'rgb(0, 0, 255, 0.1)',
tension: 0.1,
}]
}
});
)"""";

void read_th()
{
  sht30.get();

  //read temperature
  t = sht30.cTemp;

  //read humidity
  h = sht30.humidity;

  if (isnan(h) || isnan(t))
  {
    //error
#ifdef DEBUG
    Serial.println(F("Failed to read from DHT sensor!"));
#endif
    t = h = 0;
  }
  else
  {
#ifdef DEBUG
    //serial
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.print(F("°C, Humidity: "));
    Serial.print(h);
    Serial.println("%");
#endif
  }

  //save in table
  t_table[th_index] = t;
  h_table[th_index] = h;
  th_index++;

  //rewind table
  if (th_index == HT_SIZE)
  {
    //move
    for (unsigned int i = 0; i < HT_SIZE - HT_REWIND; i++)
    {
      t_table[i] = t_table[i + HT_REWIND];
      h_table[i] = h_table[i + HT_REWIND];
    }
    //zero
    for (unsigned int i = HT_SIZE - HT_REWIND; i < HT_SIZE; i++)
    {
      t_table[i] = 0;
      h_table[i] = 0;
    }
    //adjust time for labels
    base_time += 60 * 60;
    //set index
    th_index -= HT_REWIND;
  }
}

String generate_data()
{
  String s;
  s = "const t_data = [";
  //dump temp table
  for (unsigned int i = 0; i < HT_SIZE; i++)
  {
    s += String(t_table[i], 2);
    s += ",";
  }
  s += "];const h_data = [";
  //dump humidity table
  for (unsigned int i = 0; i < HT_SIZE; i++)
  {
    s += String(h_table[i], 2);
    s += ",";
  }
  s += "];const l_data = [";
  //generate empty labels
  time_t now = base_time;
  for (unsigned int i = 0; i < th_index; i++)
  {
    if (i % 60 == 0)
    {
      char buf[16];
      struct tm *ptm;
      ptm = localtime(&now);
      now += 60 * 60;
      snprintf(buf, sizeof(buf), "%02d:%02d", ptm->tm_hour, ptm->tm_min);
      s += "\"" + String(buf) + "\",";
    }
    else
    {
      s += "\"\",";
    }
  }
  s += "];";
  return s;
}

void send_html(const char *h)
{
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send_P(200, "text/html", html_header);
  if (light_state)
  {
    server.sendContent_P(light_off);
  }
  else
  {
    server.sendContent_P(light_on);
  }
  server.sendContent_P(html_header2);
  server.sendContent(h);
  server.sendContent_P(html_footer);
}

void handle_404()
{
  send_html("<p>Not found!</p>");
}

void handle_root()
{
  String s;
  s = String(html_main);
  s += "<script>";
  s += generate_data();
  s += js_script;
  s += "</script>";
  send_html(s.c_str());
}

void handle_now()
{
  String s;
  s = String(t, 2) + ";" + String(h, 2);
  server.send(200, "text/txt", s);
}

void handle_light()
{
  light_state ^= 1;
  digitalWrite(RELAY_PIN, light_state);
  server.send(200, "text/html", "<meta http-equiv='refresh' content='0; url=/' />");
}

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, light_state);

  WiFi.mode(WIFI_STA);

  Serial.begin(115200);
  delay(10);

  //wm.setDebugOutput(false);

  String device_name = "HTMON";

  WiFi.hostname(device_name);
  wm.setConfigPortalTimeout(180);

  if (!wm.autoConnect(device_name.c_str()))
  {
    ESP.restart();
    delay(1000);
  }

  httpUpdater.setup(&server, "/update");

  //install www handlers
  server.onNotFound(handle_404);
  server.on("/", handle_root);
  server.on("/now", handle_now);
  server.on("/light", handle_light);

  server.begin();

  //discovery protocols
  MDNS.begin(device_name);
  MDNS.addService("http", "tcp", 80);

  //get internet time
  configTime("<-03>3", "pool.ntp.org");
  //verifica 2021...
  while (time(nullptr) < 1609459200)
  {
    delay(100);
  }
  base_time = time(nullptr);

#ifdef DEBUG
  Serial.println();
  Serial.println(F("---------------HTMON----------------"));
  Serial.println(ctime(&base_time));
  Serial.println(F("------------------------------------"));
#endif

  memset(t_table, 0, sizeof(t_table));
  memset(h_table, 0, sizeof(h_table));
  th_index = 0;

  lastMillis = millis() - (READ_INTERVAL * 60 * 1000UL);

  display.init();
  display.flipScreenVertically();
  display.drawXbm((128 - logo_width) / 2, (64 - logo_height) / 2, logo_width, logo_height, logo);
  display.display();
  delay(1000 * 3);
}

void loop()
{
  server.handleClient();
  MDNS.update();

  if (millis() - lastMillis >= (READ_INTERVAL * 60 * 1000UL))
  {
    lastMillis = millis();
    read_th();
  }

  display.clear();

  display.setFont(ArialMT_Plain_10);
  display.drawRect(0, 0, 128, 64);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(10, 15, "Temperatura:");
  display.drawString(10, 40, "Umidade:");

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128 - 10, 15, String(t) + "°");
  display.drawString(128 - 10, 40, String(h) + "%");

  display.display();
}
