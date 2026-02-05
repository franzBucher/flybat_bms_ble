#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/sensor/sensor.h"


namespace esphome {
namespace flybat_bms_ble {

class FlyBatBmsBle : public ble_client::BLEClientNode, public Component {
 public:
  void on_ble_client_connected() override;
  void on_ble_client_disconnected() override {}
  void dump_config() override;

  // Setter für Sensoren (werden aus component.py aufgerufen)
  void set_cell1_sensor(sensor::Sensor *s) { cell1_ = s; }
  void set_cell2_sensor(sensor::Sensor *s) { cell2_ = s; }
  void set_cell3_sensor(sensor::Sensor *s) { cell3_ = s; }
  void set_cell4_sensor(sensor::Sensor *s) { cell4_ = s; }
  void set_cell5_sensor(sensor::Sensor *s) { cell5_ = s; }

  void set_voltage_sensor(sensor::Sensor *s) { voltage_ = s; }
  void set_current_sensor(sensor::Sensor *s) { current_ = s; }
  void set_temp1_sensor(sensor::Sensor *s) { temp1_ = s; }
  void set_temp2_sensor(sensor::Sensor *s) { temp2_ = s; }
  void set_soc_sensor(sensor::Sensor *s) { soc_ = s; }

  void set_charging_binary_sensor(binary_sensor::BinarySensor *b) { charging_ = b; }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *b) { discharging_ = b; }
  void set_balancing_binary_sensor(binary_sensor::BinarySensor *b) { balancing_ = b; }

 protected:
  void handle_notify_(uint8_t *data, uint16_t length);
  void parse_frame_(const std::string &frame);

  sensor::Sensor *cell1_{nullptr};
  sensor::Sensor *cell2_{nullptr};
  sensor::Sensor *cell3_{nullptr};
  sensor::Sensor *cell4_{nullptr};
  sensor::Sensor *cell5_{nullptr};

  sensor::Sensor *voltage_{nullptr};
  sensor::Sensor *current_{nullptr};
  sensor::Sensor *temp1_{nullptr};
  sensor::Sensor *temp2_{nullptr};
  sensor::Sensor *soc_{nullptr};

  binary_sensor::BinarySensor *charging_{nullptr};
  binary_sensor::BinarySensor *discharging_{nullptr};
  binary_sensor::BinarySensor *balancing_{nullptr};
};

}  // namespace flybat_bms_ble
}  // namespace esphome
