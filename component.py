import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import ble_client, sensor, binary_sensor
from esphome.const import (
    CONF_ID,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    ICON_FLASH,
    ICON_THERMOMETER,
    ICON_CURRENT_AC,
    ICON_BATTERY,
)

flybat_ns = cg.esphome_ns.namespace("flybat_bms_ble")
FlyBatBmsBle = flybat_ns.class_("FlyBatBmsBle", cg.Component, ble_client.BLEClientNode)

CONF_CELL1 = "cell_1"
CONF_CELL2 = "cell_2"
CONF_CELL3 = "cell_3"
CONF_CELL4 = "cell_4"
CONF_CELL5 = "cell_5"
CONF_VOLTAGE = "voltage"
CONF_CURRENT = "current"
CONF_TEMP1 = "temp_1"
CONF_TEMP2 = "temp_2"
CONF_SOC = "soc"

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_BALANCING = "balancing"

CONFIG_SCHEMA = ble_client.BLE_CLIENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(FlyBatBmsBle),

        cv.Optional(CONF_CELL1): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_FLASH,
            accuracy_decimals=3,
        ),
        cv.Optional(CONF_CELL2): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_FLASH,
            accuracy_decimals=3,
        ),
        cv.Optional(CONF_CELL3): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_FLASH,
            accuracy_decimals=3,
        ),
        cv.Optional(CONF_CELL4): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_FLASH,
            accuracy_decimals=3,
        ),
        cv.Optional(CONF_CELL5): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_FLASH,
            accuracy_decimals=3,
        ),

        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_BATTERY,
            accuracy_decimals=2,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_AC,
            accuracy_decimals=1,
        ),
        cv.Optional(CONF_TEMP1): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=1,
        ),
        cv.Optional(CONF_TEMP2): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=1,
        ),
        cv.Optional(CONF_SOC): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_BATTERY,
            accuracy_decimals=0,
        ),

        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await ble_client.register_ble_node(var, config)
    cg.add(var)

    if CONF_CELL1 in config:
        sens = await sensor.new_sensor(config[CONF_CELL1])
        cg.add(var.set_cell1_sensor(sens))
    if CONF_CELL2 in config:
        sens = await sensor.new_sensor(config[CONF_CELL2])
        cg.add(var.set_cell2_sensor(sens))
    if CONF_CELL3 in config:
        sens = await sensor.new_sensor(config[CONF_CELL3])
        cg.add(var.set_cell3_sensor(sens))
    if CONF_CELL4 in config:
        sens = await sensor.new_sensor(config[CONF_CELL4])
        cg.add(var.set_cell4_sensor(sens))
    if CONF_CELL5 in config:
        sens = await sensor.new_sensor(config[CONF_CELL5])
        cg.add(var.set_cell5_sensor(sens))

    if CONF_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_VOLTAGE])
        cg.add(var.set_voltage_sensor(sens))
    if CONF_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_CURRENT])
        cg.add(var.set_current_sensor(sens))
    if CONF_TEMP1 in config:
        sens = await sensor.new_sensor(config[CONF_TEMP1])
        cg.add(var.set_temp1_sensor(sens))
    if CONF_TEMP2 in config:
        sens = await sensor.new_sensor(config[CONF_TEMP2])
        cg.add(var.set_temp2_sensor(sens))
    if CONF_SOC in config:
        sens = await sensor.new_sensor(config[CONF_SOC])
        cg.add(var.set_soc_sensor(sens))

    if CONF_CHARGING in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_CHARGING])
        cg.add(var.set_charging_binary_sensor(bs))
    if CONF_DISCHARGING in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_DISCHARGING])
        cg.add(var.set_discharging_binary_sensor(bs))
    if CONF_BALANCING in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_BALANCING])
        cg.add(var.set_balancing_binary_sensor(bs))
