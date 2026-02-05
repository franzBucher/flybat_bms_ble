#include "flybat_bms_ble.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

namespace esphome {
namespace flybat_bms_ble {

static const char *TAG = "flybat_bms";

using esp32_ble_tracker::ESPBTUUID;

void FlyBatBmsBle::dump_config() {
  ESP_LOGCONFIG(TAG, "FlyBat BLE BMS");
  LOG_BLE_CLIENT(this);
}

void FlyBatBmsBle::on_ble_client_connected() {
  ESP_LOGI(TAG, "Connected to FlyBat BMS, registering notify on Nordic UART TX");

  // TX Characteristic of Nordic UART Service
  auto uuid = ESPBTUUID::from_raw("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

  this->parent()->register_for_notify(
      uuid, [this](uint16_t handle, uint8_t *data, uint16_t length) {
        this->handle_notify_(data, length);
      });
}

void FlyBatBmsBle::handle_notify_(uint8_t *data, uint16_t length) {
  if (length == 0)
    return;

  std::string frame(reinterpret_cast<char *>(data), length);
  ESP_LOGV(TAG, "Notify frame: %s", frame.c_str());

  // Erwartetes Format: ASCII, beginnt mit ':' und endet mit '~'
  if (frame.size() < 10 || frame.front() != ':' || frame.back() != '~') {
    ESP_LOGV(TAG, "Frame format not recognized");
    return;
  }

  this->parse_frame_(frame);
}

// *** Achtung: Offsets sind aus deinen Dumps grob abgeleitet und evtl. anzupassen ***
static int parse_hex_field(const std::string &s, size_t pos, size_t len) {
  if (pos + len > s.size())
    return 0;
  return static_cast<int>(strtol(s.substr(pos, len).c_str(), nullptr, 16));
}

void FlyBatBmsBle::parse_frame_(const std::string &frame) {
  // Beispiel‑Frame (gekürzt):
  // :0082310080000101C00000880d8d0d7c0da00d9f0000....61FB~
  //
  // Wir ignorieren zunächst die ersten 10–12 Zeichen (Header) und arbeiten
  // mit festen Offsets im ASCII‑Hex‑String. Das ist nur ein Startpunkt.

  // Header überspringen (":0082310080" = 10 Zeichen)
  const size_t base = 10;

  // Jede Zelle = 4 ASCII‑Hex‑Zeichen (2 Bytes)
  // Offsets hier sind heuristisch – bitte bei Bedarf anpassen.
  int cell1_mv = parse_hex_field(frame, base + 16, 4);  // Beispiel‑Offset
  int cell2_mv = parse_hex_field(frame, base + 20, 4);
  int cell3_mv = parse_hex_field(frame, base + 24, 4);
  int cell4_mv = parse_hex_field(frame, base + 28, 4);
  int cell5_mv = parse_hex_field(frame, base + 32, 4);

  int pack_mv = parse_hex_field(frame, base + 72, 4);   // Gesamtspannung
  int current_0_1a = parse_hex_field(frame, base + 76, 4);  // Strom * 0.1 A
  int temp1_0_1c = parse_hex_field(frame, base + 80, 4);
  int temp2_0_1c = parse_hex_field(frame, base + 84, 4);

  // Beispiel‑SOC (am Ende des Frames, 4 Hex‑Zeichen vor der Checksumme)
  int soc_pct = 0;
  if (frame.size() > 12) {
    soc_pct = parse_hex_field(frame, frame.size() - 8, 4);
  }

  if (cell1_ != nullptr)
    cell1_->publish_state(cell1_mv / 1000.0f);
  if (cell2_ != nullptr)
    cell2_->publish_state(cell2_mv / 1000.0f);
  if (cell3_ != nullptr)
    cell3_->publish_state(cell3_mv / 1000.0f);
  if (cell4_ != nullptr)
    cell4_->publish_state(cell4_mv / 1000.0f);
  if (cell5_ != nullptr)
    cell5_->publish_state(cell5_mv / 1000.0f);

  if (voltage_ != nullptr)
    voltage_->publish_state(pack_mv / 100.0f);

  float current_a = current_0_1a / 10.0f;
  if (current_ != nullptr)
    current_->publish_state(current_a);

  if (temp1_ != nullptr)
    temp1_->publish_state(temp1_0_1c / 10.0f);
  if (temp2_ != nullptr)
    temp2_->publish_state(temp2_0_1c / 10.0f);

  if (soc_ != nullptr)
    soc_->publish_state(soc_pct);

  if (charging_ != nullptr)
    charging_->publish_state(current_a > 0.1f);
  if (discharging_ != nullptr)
    discharging_->publish_state(current_a < -0.1f);

  // Balancing‑Flag ist im Protokoll noch nicht sicher – hier nur Dummy:
  if (balancing_ != nullptr)
    balancing_->publish_state(false);
}

}  // namespace flybat_bms_ble
}  // namespace esphome
