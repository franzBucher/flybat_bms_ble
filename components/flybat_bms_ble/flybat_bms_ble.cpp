#include "flybat_bms_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace flybat_bms_ble {

static const char *TAG = "flybat_bms_ble";

void FlyBatBmsBle::gattc_event_handler(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param) {

  switch (event) {

    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "BLE connected");
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGI(TAG, "BLE disconnected");
      notify_handle_ = 0;
      break;

    case ESP_GATTC_SEARCH_CMPL_EVT:
      ESP_LOGI(TAG, "Service discovery complete");
      break;

    case ESP_GATTC_NOTIFY_EVT:
      ESP_LOGD(TAG, "Notify received (%d bytes)", param->notify.value_len);
      // Parser kommt im nächsten Schritt
      break;

    default:
      break;
  }
}

}  // namespace flybat_bms_ble
}  // namespace esphome
