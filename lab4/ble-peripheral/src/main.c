#include <stdbool.h>
#include <zephyr/types.h>
#include <zephyr/drivers/sensor.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>


#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>


#define LAB4_SERVICE_UUID_VAL \
 BT_UUID_128_ENCODE(0x5253FF4B, 0xE47C, 0x4EC8, 0x9792, 0x69FDF4923B13)
#define LAB4_SERVICE_UUID_CONST_VAL \
 BT_UUID_128_ENCODE(0x5253FF13, 0xE47C, 0x4EC8, 0x9792, 0x69FDF4923B13)
#define LAB4_SERVICE_UUID_COUNTER_VAL \
 BT_UUID_128_ENCODE(0x5253FF4D, 0xE47C, 0x4EC8, 0x9792, 0x69FDF4923B13)


#define LAB4_SERVICE_UUID BT_UUID_DECLARE_128(LAB4_SERVICE_UUID_VAL)
#define LAB4_SERVICE_UUID_CONST BT_UUID_DECLARE_128(LAB4_SERVICE_UUID_CONST_VAL)
#define LAB4_SERVICE_UUID_COUNTER BT_UUID_DECLARE_128(LAB4_SERVICE_UUID_COUNTER_VAL)


static ssize_t const_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);


// Global value that saves state for the characteristic.
uint32_t const_value = 0xc5ec4501;
uint32_t value1 = 0x0;


// Set up the advertisement data.
#define DEVICE_NAME "EmbWIoT_ASBMSIA_13"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)


static const struct bt_data ad[] = {
   BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};


static const struct bt_data sd[] = {
   BT_DATA_BYTES(BT_DATA_UUID128_ALL, LAB4_SERVICE_UUID_VAL),
};


// Setup the the service and characteristics.
BT_GATT_SERVICE_DEFINE(lab4_service,
   BT_GATT_PRIMARY_SERVICE(LAB4_SERVICE_UUID),
   BT_GATT_CHARACTERISTIC(LAB4_SERVICE_UUID_CONST, BT_GATT_CHRC_READ,
                  BT_GATT_PERM_READ, const_read, NULL, &const_value),
   BT_GATT_CHARACTERISTIC(LAB4_SERVICE_UUID_COUNTER, BT_GATT_CHRC_READ,
                  BT_GATT_PERM_READ, const_read, NULL, &value1)
);




// Callback when a client reads the characteristic.
//
// Documented under name "bt_gatt_attr_read_chrc()"
static ssize_t const_read(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf,
                                  uint16_t len,
                                  uint16_t offset)
{
   // The `user_data` corresponds to the pointer provided as the last "argument"
   // to the `BT_GATT_CHARACTERISTIC` macro.
   uint32_t *value = attr->user_data;


   // User helper function to encode the output data to send to
   // the client.
   ssize_t ret =bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));


   if (ret >= 0) {
       if (value == &value1) {
           value1++;   // Only increment this one
       }
   }


   return ret;
}




// Setup callbacks when devices connect and disconnect.
static void connected(struct bt_conn *conn, uint8_t err)
{
   if (err) {
       printk("Connection failed (err 0x%02x)\n", err);
   } else {
       printk("Connected\n");
   }
}


static void disconnected(struct bt_conn *conn, uint8_t reason)
{
   printk("Disconnected (reason 0x%02x)\n", reason);
}


static void recycled_cb(void)
{
   printk("Connection object available from previous conn. Disconnect is complete!\n");
   int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));


   if (err) {
       printk("Advertising failed to start (err %d)\n", err);
       return;
   }


   printk("Advertising successfully started\n");
}


BT_CONN_CB_DEFINE(conn_callbacks) = {
   .connected = connected,
   .disconnected = disconnected,
   .recycled = recycled_cb,
};


static void bt_ready(int err)
{
   if (err) {
       printk("Bluetooth init failed (err %d)\n", err);
       return;
   }


   printk("Bluetooth initialized\n");


   err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
   if (err) {
       printk("Advertising failed to start (err %d)\n", err);
       return;
   }


   printk("Advertising successfully started\n");
}


int main(void)
{
   int err;


   err = bt_enable(bt_ready);
   if (err) {
       printk("Bluetooth init failed (err %d)\n", err);
       return 0;
   }
}
