#ifndef __ESP_NOW_CPP__
#define __ESP_NOW_CPP__

#include "esp_now_class.h"

static void ESPNOWOnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
	ESP_LOGI("ESP-NOW", "Received from: %02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]); 
	
	memset(recv_buff, 0, sizeof(recv_buff));
	memcpy(recv_buff, data, data_len);
	
	if (recv_cb) {
		recv_cb();
	}
}

void ESPNOWOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  ESP_LOGI("ESP-NOW", "Send to %02x:%02x:%02x:%02x:%02x:%02x, Status: %s",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

ESP_NOW_CLASS::ESP_NOW_CLASS() { }

void ESP_NOW_CLASS::init(void) {
	// clear error flag
	error = false;
	// set initialized flag
	initialized = true;
	
	// Debug
	esp_log_level_set("*", ESP_LOG_INFO);
	
	// Setup WiFi
	tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );

	// Init
	ESP_ERROR_CHECK(esp_now_init());
	
	// Add peer
	esp_now_peer_info_t slave;
	slave.channel = 1;
	slave.encrypt = 0;
	memcpy(slave.peer_addr, macAddr, sizeof(macAddr));
	ESP_ERROR_CHECK(esp_now_add_peer(&slave));
	
	// Register callback
	esp_now_register_recv_cb(ESPNOWOnDataRecv);
	ESP_LOGI("ESP-NOW", "register callback");
}

int ESP_NOW_CLASS::prop_count(void) {
	// not supported
	return 0;
}

bool ESP_NOW_CLASS::prop_name(int index, char *name) {
	// not supported
	return false;
}

bool ESP_NOW_CLASS::prop_unit(int index, char *unit) {
	// not supported
	return false;
}

bool ESP_NOW_CLASS::prop_attr(int index, char *attr) {
	// not supported
	return false;
}

bool ESP_NOW_CLASS::prop_read(int index, char *value) {
	// not supported
	return false;
}

bool ESP_NOW_CLASS::prop_write(int index, char *value) {
	// not supported
	return false;
}
// --------------------------------------

void ESP_NOW_CLASS::process(Driver *drv) {

}

// Method
void ESP_NOW_CLASS::send(char* str) {
	esp_err_t result = esp_now_send(macAddr, (uint8_t*)str, strlen(str));
	if (result == ESP_OK) {
		ESP_LOGI("ESP-NOW", "Send OK");
	} else {
		ESP_LOGI("ESP-NOW", "Send fail");
	}
}

void ESP_NOW_CLASS::send(float n) {
	char s_buff[255];
	sprintf(s_buff, "%f", n);
	esp_err_t result = esp_now_send(macAddr, (uint8_t*)s_buff, strlen(s_buff));
	if (result == ESP_OK) {
		ESP_LOGI("ESP-NOW", "Send OK");
	} else {
		ESP_LOGI("ESP-NOW", "Send fail");
	}
}

void ESP_NOW_CLASS::onRecv(ESPNOWRecvCallback fn) {
	recv_cb = fn;
}

char* ESP_NOW_CLASS::readString() {
	return recv_buff;
}

float ESP_NOW_CLASS::readNumber() {
	float n = 0;
	sscanf(recv_buff, "%f", &n);
	return n;
}

#endif