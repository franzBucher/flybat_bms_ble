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




static const uint8_t FLYBAT_NOTIFY_UUID[16] = {
  0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,
  0x00,0x10,0x00,0x00,0xFF,0xE1,0x00,0x00
};

static const uint8_t NUS_SERVICE_UUID[16] = {
  0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
  0x93,0xF3,0xA3,0xB5,0x01,0x00,0x40,0x6E
};

static const uint8_t NUS_TX_UUID[16] = {
  0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
  0x93,0xF3,0xA3,0xB5,0x03,0x00,0x40,0x6E
};


static const uint8_t NUS_RX_UUID[16] = {
  0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
  0x93,0xF3,0xA3,0xB5,0x02,0x00,0x40,0x6E
};


static esp_bt_uuid_t make_uuid128_(const uint8_t u[16]) {
  esp_bt_uuid_t out{};
  out.len = ESP_UUID_LEN_128;
  memcpy(out.uuid.uuid128, u, 16);
  return out;
}




void FlyBatBmsBle::gattc_event_handler(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param) {

  switch (event) {

    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "BLE connected");
      gattc_if_ = gattc_if;
      conn_id_ = param->connect.conn_id;
      break;

    // case ESP_GATTC_SEARCH_RES_EVT: {
    //   auto &srvc = param->search_res;

    // if (srvc.srvc_id.uuid.len == ESP_UUID_LEN_128 &&
    //   memcmp(srvc.srvc_id.uuid.uuid.uuid128,
    //        FLYBAT_NOTIFY_UUID, 16) == 0) {
    //    notify_handle_ = srvc.start_handle;
    //     ESP_LOGI(TAG, "Found FlyBat notify handle: 0x%04X", notify_handle_);
    //   }
    //   break;
    // }
    case ESP_GATTC_SEARCH_RES_EVT: {
      auto &res = param->search_res;

      if (res.srvc_id.uuid.len == ESP_UUID_LEN_128 &&
          memcmp(res.srvc_id.uuid.uuid.uuid128, NUS_SERVICE_UUID, 16) == 0) {

        service_start_ = res.start_handle;
        service_end_ = res.end_handle;
        ESP_LOGI(TAG, "Found NUS service: 0x%04X-0x%04X", service_start_, service_end_);
      }
      break;
    }


    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGI(TAG, "Service discovery complete");

      if (service_start_ == 0) {
        ESP_LOGE(TAG, "NUS service not found");
        return;
      }

      esp_bt_uuid_t tx_uuid = make_uuid128_(NUS_TX_UUID);

      esp_gattc_char_elem_t result[1];
      uint16_t count = 1;

      esp_err_t err = esp_ble_gattc_get_char_by_uuid(
          gattc_if_,
          conn_id_,
          service_start_,
          service_end_,
          tx_uuid,
          result,
          &count);

      if (err != ESP_OK || count == 0) {
        ESP_LOGE(TAG, "NUS TX characteristic not found (err=%d, count=%u)", (int) err, (unsigned) count);
        return;
      }

      notify_handle_ = result[0].char_handle;
      ESP_LOGI(TAG, "Found NUS TX notify handle: 0x%04X", notify_handle_);

      esp_ble_gattc_register_for_notify(
          gattc_if_,
          this->parent()->get_remote_bda(),
          notify_handle_);
      esp_bt_uuid_t rx_uuid = make_uuid128_(NUS_RX_UUID);

      esp_gattc_char_elem_t rx_result[1];
      uint16_t rx_count = 1;

      esp_err_t rx_err = esp_ble_gattc_get_char_by_uuid(
          gattc_if_, conn_id_, service_start_, service_end_, rx_uuid, rx_result, &rx_count);

      if (rx_err == ESP_OK && rx_count > 0) {
        rx_handle_ = rx_result[0].char_handle;
        ESP_LOGI(TAG, "Found NUS RX write handle: 0x%04X", rx_handle_);
      } else {
        ESP_LOGE(TAG, "NUS RX characteristic not found (err=%d, count=%u)", (int) rx_err, (unsigned) rx_count);
      }

      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      uint16_t notify_en = 1;

      // Bei vielen NUS-Implementierungen sitzt CCCD direkt hinter der Char-Value-Handle-Adresse,
      // aber sauberer ist "get_descr_by_char_handle". Das machen wir im nächsten Schritt, falls nötig.
      esp_ble_gattc_write_char_descr(
          gattc_if_,
          conn_id_,
          param->reg_for_notify.handle + 1,
          sizeof(notify_en),
          (uint8_t *) &notify_en,
          ESP_GATT_WRITE_TYPE_RSP,
          ESP_GATT_AUTH_REQ_NONE);
          if (rx_handle_ != 0) {
            uint8_t req[1] = {0x00};
            esp_ble_gattc_write_char(
                gattc_if_,
                conn_id_,
                rx_handle_,
                sizeof(req),
                req,
                ESP_GATT_WRITE_TYPE_RSP,
                ESP_GATT_AUTH_REQ_NONE);
            ESP_LOGI(TAG, "Sent test request to RX");
          } else {
            ESP_LOGW(TAG, "RX handle not known, cannot send request");
          }


      ESP_LOGI(TAG, "UART notifications enable requested");
      break;
    }

    case ESP_GATTC_NOTIFY_EVT: {
      const auto *v = param->notify.value;
      const int n = param->notify.value_len;

      ESP_LOGI(TAG, "Notify (%d bytes)", n);

      char line[3 * 32 + 1];
      int idx = 0;
      for (int i = 0; i < n; i++) {
        idx += snprintf(line + idx, sizeof(line) - idx, "%02X ", v[i]);
        if ((i % 32) == 31 || i == n - 1) {
          line[idx] = 0;
          ESP_LOGI(TAG, "  %s", line);
          idx = 0;
        }
      }
      break;
    }
      
      break;

    default:
      break;
  }
}

}  // namespace flybat_bms_ble
}  // namespace esphome


