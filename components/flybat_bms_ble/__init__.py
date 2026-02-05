import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client, sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["ble_client", "esp32_ble_tracker"]

flybat_ns = cg.esphome_ns.namespace("flybat_bms_ble")
FlyBatBmsBle = flybat_ns.class_("FlyBatBmsBle", cg.Component, ble_client.BLEClientNode)

CONF_CELL_1 = "cell_1"
CONF_CELL_2 = "cell_2"
CONF_CELL_3 = "cell_3"
CONF_CELL_4 = "cell_4"
CONF_CELL_5 = "cell_5"
CONF_VOLTAGE = "voltage"

CONFIG_SCHEMA = ble_client.BLE_CLIENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(FlyBatBmsBle),
        cv.Optional(CONF_CELL_1): sensor.sensor_schema(),
        cv.Optional(CONF_CELL_2): sensor.sensor_schema(),
        cv.Optional(CONF_CELL_3): sensor.sensor_schema(),
        cv.Optional(CONF_CELL_4): sensor.sensor_schema(),
        cv.Optional(CONF_CELL_5): sensor.sensor_schema(),
        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await ble_client.register_ble_node(var, config)
    await cg.register_component(var, config)

    if CONF_CELL_1 in config:
        cg.add(var.set_cell1(await sensor.new_sensor(config[CONF_CELL_1])))
    if CONF_CELL_2 in config:
        cg.add(var.set_cell2(await sensor.new_sensor(config[CONF_CELL_2])))
    if CONF_CELL_3 in config:
        cg.add(var.set_cell3(await sensor.new_sensor(config[CONF_CELL_3])))
    if CONF_CELL_4 in config:
        cg.add(var.set_cell4(await sensor.new_sensor(config[CONF_CELL_4])))
    if CONF_CELL_5 in config:
        cg.add(var.set_cell5(await sensor.new_sensor(config[CONF_CELL_5])))
    if CONF_VOLTAGE in config:
        cg.add(var.set_voltage(await sensor.new_sensor(config[CONF_VOLTAGE])))
