#ifndef __WIFI_CONFIG_H
#define __WIFI_CONFIG_H

#define WIFI_SSID      "WIFI_SSID"
#define WIFI_PASSWORD  "WIFI_PASSWORD"

void my_wifi_init();

String get_local_ip();
String get_host_name();

#endif