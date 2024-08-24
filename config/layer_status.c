#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zmk/keymap.h>

static struct bt_uuid_128 layer_status_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0));

static uint8_t layer_status;

static ssize_t read_layer_status(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset) {
    layer_status = zmk_keymap_highest_layer_active();
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &layer_status,
                             sizeof(layer_status));
}

BT_GATT_SERVICE_DEFINE(layer_status_svc,
    BT_GATT_PRIMARY_SERVICE(&layer_status_uuid),
    BT_GATT_CHARACTERISTIC(&layer_status_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ, read_layer_status, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void layer_status_update(void) {
    layer_status = zmk_keymap_highest_layer_active();
    bt_gatt_notify(NULL, &layer_status_svc.attrs[1], &layer_status, sizeof(layer_status));
}
