#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace flybat_bms_ble {

class FlyBatBmsBle : public Component, public ble_client::BLEClientNode {
 public:
  void set_cell1(sensor::Sensor *s) { cell1_ = s; }
  void set_cell2(sensor::Sensor *s) { cell2_ = s; }
  void set_cell3(sensor::Sensor *s) { cell3_ = s; }
  void set_cell4(sensor::Sensor *s) { cell4_ = s; }
  void set_cell5(sensor::Sensor *s) { cell5_ = s; }
  void set_voltage(sensor::Sensor *s) { voltage_ = s; }

  void gattc_event_handler(
      esp_gattc_cb_event_t event,
      esp_gatt_if_t gattc_if,
      esp_ble_gattc_cb_param_t *param) override;

 protected:
  sensor::Sensor *cell1_{nullptr};
  sensor::Sensor *cell2_{nullptr};
  sensor::Sensor *cell3_{nullptr};
  sensor::Sensor *cell4_{nullptr};
  sensor::Sensor *cell5_{nullptr};
  sensor::Sensor *voltage_{nullptr};

protected:
  uint16_t notify_handle_{0};
  esp_gatt_if_t gattc_if_{0};
  uint16_t conn_id_{0};
  uint16_t service_start_{0};
  uint16_t service_end_{0};
  uint16_t rx_handle_{0};


};

}  // namespace flybat_bms_ble
}  // namespace esphome
