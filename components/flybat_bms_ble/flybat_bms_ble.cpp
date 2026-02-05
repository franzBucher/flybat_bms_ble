#include "flybat_bms_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace flybat_bms_ble {

static const char *TAG = "flybat_bms_ble";

static const esp_bt_uuid_t SERVICE_UUID = {
  .len = ESP_UUID_LEN_128,
  .uuid = {.uuid128 = {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFF,0xE0,0x00,0x00}}
};

static const esp_bt_uuid_t NOTIFY_UUID = {
  .len = ESP_UUID_LEN_128,
  .uuid = {.uuid128 = {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFF,0xE1,0x00,0x00}}
};



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

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGI(TAG, "Service discovery complete");
    
      auto *chr = this->parent()->get_characteristic(SERVICE_UUID, NOTIFY_UUID);
      if (chr == nullptr) {
        ESP_LOGE(TAG, "Notify characteristic not found");
        return;
      }
    
      notify_handle_ = chr->handle;
      ESP_LOGI(TAG, "Notify handle: 0x%04X", notify_handle_);
    
      esp_ble_gattc_register_for_notify(
          gattc_if,
          this->parent()->get_remote_bda(),
          notify_handle_);

      break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        uint16_t notify_en = 1;
        esp_ble_gattc_write_char_descr(
            gattc_if,
            param->reg_for_notify.conn_id,
            param->reg_for_notify.handle + 1,  // CCCD
            sizeof(notify_en),
            (uint8_t *)&notify_en,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE);
      
        ESP_LOGI(TAG, "Notifications enabled");
        break;
}

}

    default:
      break;
  }
}

}  // namespace flybat_bms_ble
}  // namespace esphome


