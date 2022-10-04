/* main.c
 *
 * Mqtt <-> ESP32 <-> ModBus TCP Slave
*/


#include <stdio.h>
/* nvs_flash.h
 * -nvs_flash_init();
*/
#include "nvs_flash.h"
/* esp_log.h
 * ESP_LOGI(); info
 * ESP_LOGW(); warning
 * ESP_LOGE(); error
*/
#include "esp_log.h"
/* esp_wifi.h
 * - esp_netif_init();
 * - esp_event_loop_create_default();
*/
#include "esp_wifi.h"
/* protocol_examples_common.h
 * -example_connect();
 * -get_example_netif();
*/
#include "protocol_examples_common.h"
/* mbcontroller.h
 * mb_communication_info_t
*/
#include "mbcontroller.h"
/* modbus_params.h
 * input_reg_params_t
*/
#include "modbus_params.h"
/* mqtt_client.h
 * esp_mqtt_client_config_t
*/
#include "mqtt_client.h"
/* cJSON.h
 * cJSON
*/
#include "cJSON.h"
static const char *TAG = "Main";

#define MB_TCP_PORT                     (CONFIG_FMB_TCP_PORT_DEFAULT)   // TCP port used for modbus slave
#define MB_PORT_NUM     (CONFIG_MB_UART_PORT_NUM)   // Number of UART port used for Modbus connection
#define MB_DEV_SPEED    (CONFIG_MB_UART_BAUD_RATE)  // The communication speed of the UART


// The macro to get offset for parameter in the appropriate structure
#define INPUT_OFFSET(field) ((uint16_t)(offsetof(input_reg_params_t, field) + 1))
#define STR(fieldname) ((const char*)( fieldname ))
// Options can be used as bit masks or parameter limits
#define OPTS(min_val, max_val, step_val) { .opt1 = min_val, .opt2 = max_val, .opt3 = step_val }

// Example Data (Object) Dictionary for Modbus parameters:
// The CID field in the table must be unique.
// Modbus Slave Addr field defines slave address of the device with correspond parameter.
// Modbus Reg Type - Type of Modbus register area (Holding register, Input Register and such).
// Reg Start field defines the start Modbus register number and Reg Size defines the number of registers for the characteristic accordingly.
// The Instance Offset defines offset in the appropriate parameter structure that will be used as instance to save parameter value.
// Data Type, Data Size specify type of the characteristic and its data size.
// Parameter Options field specifies the options that can be used to process parameter value (limits or masks).
// Access Mode - can be used to implement custom options for processing of characteristic (Read/Write restrictions, factory mode values and etc).
const mb_parameter_descriptor_t device_parameters[] = {
    // { CID, Param Name, Units, Modbus Slave Addr, Modbus Reg Type, Reg Start, Reg Size, Instance Offset, Data Type, Data Size, Parameter Options, Access Mode}
    { 0, STR("Test"), STR("%rH"), 0, MB_PARAM_INPUT, 0, 74,
            INPUT_OFFSET(input_data0), PARAM_TYPE_U16, 1, OPTS( 0, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER}
};
// Calculate number of parameters in the table
const uint16_t num_device_parameters = (sizeof(device_parameters) / sizeof(device_parameters[0]));

#if CONFIG_MB_COMM_MODE_TCP
// This table represents slave IP addresses that correspond to the short address field of the slave in device_parameters structure
// Modbus TCP stack shall use these addresses to be able to connect and read parameters from slave
char* slave_ip_address_table[] = {
    CONFIG_SLAVE_IP_ADDRESS,
    NULL
};

const size_t ip_table_sz = (size_t)(sizeof(slave_ip_address_table) / sizeof(slave_ip_address_table[0]));
#endif

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_to_json(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_publish(event->client, CONFIG_MQTT_STATUS, "----handle", 0,0,0);
    int16_t fc,unitid, address, quantity;
    char *topic = NULL;

    cJSON *mqtt_request;
    mqtt_request = cJSON_ParseWithLength(event->data,event->data_len);
    const cJSON *fc_J = NULL;fc_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"fc");
    fc =(int)cJSON_GetNumberValue(fc_J);
    const cJSON *unitid_J = NULL;unitid_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"unitid");
    unitid =(int)cJSON_GetNumberValue(unitid_J);
    const cJSON *address_J = NULL;address_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"address");
    address =(int)cJSON_GetNumberValue(address_J);
    const cJSON *quantity_J = NULL;quantity_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"quantity");
    quantity =(int)cJSON_GetNumberValue(quantity_J);

    int16_t value[quantity+1];  //Value to set or Variable for Output
    const cJSON *value_J = NULL;value_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"value");
    for (size_t i = 0; i < quantity; i++) {
        value[i]=0;
        if(cJSON_IsArray(value_J)){
            cJSON *valu_J = NULL;valu_J=cJSON_GetArrayItem(value_J,i);
            value[i]=(int16_t)cJSON_GetNumberValue(valu_J);
        }
        }

    const cJSON *topic_J = NULL;topic_J = cJSON_GetObjectItemCaseSensitive(mqtt_request,"topic");
    topic =strdup(topic_J->valuestring);

    cJSON_Delete(mqtt_request);

     printf("topic   ='%s'\r\n",topic); // topic
     printf("fc      =%i\r\n", fc); // fc (mb_param_type Modbus Register Type)
     printf("unitid  =%i\r\n", unitid); // unitid (mb_param_type Modbus addres)
     printf("address =%i\r\n", address);/*!< This is the Modbus register address. This is the 0 based value. */
     printf("quantity=%i\r\n", quantity);/*!< Size of mb parameter in registers */

    esp_mqtt_client_publish(event->client, CONFIG_MQTT_STATUS , "---handle-", 0,0,0);
    esp_err_t err = ESP_OK;
    mb_param_request_t setparam = {unitid, fc, address, quantity};
     /*
      * Device Adress,
      * Func code (3 Read Holding Reg, 6 Write Input Reg),  (de fc in node red)
      * start Adr,
      * reg length
    */

    if (fc==6){// ||fc==2 // te testen
        int16_t var=value[0];
        err = mbc_master_send_request(&setparam, &var);
        value[0]=var;
    } else {
        err = mbc_master_send_request(&setparam, value);
    }

    if(err==ESP_OK) {
        esp_mqtt_client_publish(event->client, CONFIG_MQTT_STATUS , "--handle--", 0,0,0);

        ESP_LOGI(TAG, "Set to Value =");

        cJSON *json_uit = cJSON_CreateObject();
        cJSON *data = cJSON_CreateArray();

        for (int i = 0; i < (quantity); i++)
        {
            cJSON_AddNumberToObject(data,"data",value[i]);
        }

        cJSON_AddItemToObject(json_uit,"data",data);

        cJSON_AddStringToObject(json_uit,"topic",topic);
        cJSON *modusrequest= cJSON_CreateObject();;
        cJSON_AddItemToObject(json_uit,"modbusRequest",modusrequest);
        cJSON_AddNumberToObject(modusrequest,"fc",fc);
        cJSON_AddNumberToObject(modusrequest,"address",address);
        cJSON_AddNumberToObject(modusrequest,"quantity",quantity);

        /*
         * topic
         * payload
         * - data
         * - values
         * - buffer
         * - topic
         * - modusrequest
         *   - unitid
         *   - fc
         *   - address
         *   - quantity
        */
        char *json_string = cJSON_Print(json_uit);
        if (json_string)
        {
            int msg_id =0;
            if (fc<5){
                msg_id = esp_mqtt_client_publish(event->client, CONFIG_MQTT_RESPONSE_READ, json_string, strlen(json_string), 0,0);
            }else {
                msg_id = esp_mqtt_client_publish(event->client, CONFIG_MQTT_RESPONSE_WRITE, json_string, strlen(json_string), 0,0);
            }
            esp_mqtt_client_publish(event->client, CONFIG_MQTT_STATUS , "-handle---", 0,0,0);
            printf("json_string send(%i):\n%s\n",strlen(json_string), json_string);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            cJSON_free(json_string);
        }
        cJSON_Delete(json_uit);

    } else if (err==ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "invalid Argument");
    }

    free(topic);
    esp_mqtt_client_publish(event->client, CONFIG_MQTT_STATUS , "handle----", 0,0,0);

}

/* subscribe_to_broker
 *
 * - esp_mqtt_client_subscribe => krijg data
 * - esp_mqtt_client_unsubscribe => stop met de data te krijgen
 *
 * - esp_mqtt_client_publish => send data
 *
*/
static void  subscribe_to_broker(esp_mqtt_client_handle_t client){
    int msg_id;

    msg_id = esp_mqtt_client_subscribe(client, CONFIG_MQTT_RQUEST_READ, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    msg_id = esp_mqtt_client_subscribe(client, CONFIG_MQTT_RQUEST_WRITE, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    esp_mqtt_client_publish(client, CONFIG_MQTT_STATUS, "init", 0,0,0);
}

/* mqtt_event_handler
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {

    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        subscribe_to_broker(client);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        mqtt_to_json(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void app_main(void)
{

    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      result = nvs_flash_init();
    }
    if(result != ESP_OK){
        ESP_LOGE(TAG,"nvs_flash_init fail, returns(0x%x).",(uint32_t)result);
    }

    result = esp_netif_init();
    if(result != ESP_OK){
        ESP_LOGE(TAG,"esp_netif_init fail, returns(0x%x).",(uint32_t)result);
    }

    result = esp_event_loop_create_default();
    if(result != ESP_OK){
        ESP_LOGE(TAG,"esp_event_loop_create_default fail, returns(0x%x).",(uint32_t)result);
    }
    // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    // Read "Establishing Wi-Fi or Ethernet Connection" section in
    // examples/protocols/README.md for more information about this function.
    result = example_connect();
    if(result != ESP_OK){
        ESP_LOGE(TAG,"example_connect fail, returns(0x%x).",(uint32_t)result);
    }
        ESP_LOGI(TAG,"example_connect ok, returns(0x%x).",(uint32_t)result);

    mb_communication_info_t comm_info = {};
#if CONFIG_MB_COMM_MODE_TCP
    comm_info.ip_port = MB_TCP_PORT;
    comm_info.ip_addr_type = MB_IPV4;
    comm_info.ip_mode = MB_MODE_TCP;
    comm_info.ip_addr = (void*)slave_ip_address_table;
    comm_info.ip_netif_ptr = (void*)get_example_netif();
#else
    comm_info.port= MB_PORT_NUM;
#if CONFIG_MB_COMM_MODE_ASCII
    comm_info.mode = MB_MODE_ASCII;
#elif CONFIG_MB_COMM_MODE_RTU
    comm_info.mode = MB_MODE_RTU;
#endif
    comm_info.baudrate = MB_DEV_SPEED;
    comm_info.parity = MB_PARITY_NONE;
#endif

    void* master_handler = NULL;

#if CONFIG_MB_COMM_MODE_TCP
    result = mbc_master_init_tcp(&master_handler);
#else
        ESP_LOGI(TAG,"mbc_master_init");
    result = mbc_master_init(MB_PORT_SERIAL_MASTER,&master_handler);
#endif
    if(master_handler == NULL){
        ESP_LOGE(TAG,"mb controller initialization fail.");
    }
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb controller initialization fail, returns(0x%x).",(uint32_t)result);
    }

        ESP_LOGI(TAG,"mbc_master_setup");
    result = mbc_master_setup((void*)&comm_info);
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb controller setup fail, returns(0x%x).",(uint32_t)result);
    }
#if CONFIG_MB_COMM_MODE_TCP
    ESP_LOGI(TAG, "Modbus master mbc_master_setup pord = %i",comm_info.ip_port);
#else
        ESP_LOGI(TAG,"Set UART pin numbers");
    // Set UART pin numbers
    result = uart_set_pin(MB_PORT_NUM, CONFIG_MB_UART_TXD, CONFIG_MB_UART_RXD,
                              CONFIG_MB_UART_RTS, UART_PIN_NO_CHANGE);
        ESP_LOGI(TAG,"mb controller setup TXDpin %x RXDpin %x RTSpin %x pinCHANGE %x",
        CONFIG_MB_UART_TXD,CONFIG_MB_UART_RXD,CONFIG_MB_UART_RTS,UART_PIN_NO_CHANGE);
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb serial set pin failure, uart_set_pin() returned (0x%x).",(uint32_t)result);
    }
#endif


        ESP_LOGI(TAG,"mbc_master_start");
    result = mbc_master_start();
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb controller start fail, returns(0x%x).",(uint32_t)result);
    }

#if CONFIG_MB_COMM_MODE_TCP
#else
    // Set driver mode to Half Duplex
    result = uart_set_mode(MB_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX);
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb serial set mode failure, uart_set_mode() returned (0x%x).",(uint32_t)result);
    }
    vTaskDelay(5);
#endif

    result = mbc_master_set_descriptor(&device_parameters[0], num_device_parameters);
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mb controller set descriptor fail, returns(0x%x).",(uint32_t)result);
    }
    ESP_LOGI(TAG, "Modbus master stack initialized...");

    vTaskDelay(50);

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    while (true) {
        printf(".");
        TickType_t xDelay = 500 / portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
    }

    int msg_id;
    msg_id = esp_mqtt_client_unsubscribe(client, CONFIG_MQTT_RQUEST_READ);
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_unsubscribe(client, CONFIG_MQTT_RQUEST_WRITE);
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);


    result = mbc_master_destroy();
    if(result != ESP_OK){
        ESP_LOGE(TAG,"mbc_master_destroy fail, returns(0x%x).",(uint32_t)result);
    }
    ESP_LOGI(TAG, "Modbus master stack destroy...");

    result = ESP_OK;
#if CONFIG_MB_COMM_MODE_TCP
    for (int i = 0; ((i < ip_table_sz) && slave_ip_address_table[i] != NULL); i++) {
        if (slave_ip_address_table[i]) {
            free(slave_ip_address_table[i]);
            slave_ip_address_table[i] = "FROM_STDIN";
        }
    }
#endif

    result = example_disconnect();
    if(result != ESP_OK){
        ESP_LOGE(TAG, "example_disconnect fail, returns(0x%x).",(uint32_t)result);
    }
    result = esp_event_loop_delete_default();
    if(result != ESP_OK){
        ESP_LOGE(TAG, "esp_event_loop_delete_default fail, returns(0x%x).",(uint32_t)result);
    }
    result = esp_netif_deinit();
    if(result != ESP_OK || result != ESP_ERR_NOT_SUPPORTED){
        ESP_LOGE(TAG, "esp_netif_deinit fail, returns(0x%x).",(uint32_t)result);
    }
    result = nvs_flash_deinit();
    if(result != ESP_OK){
        ESP_LOGE(TAG, "nvs_flash_deinit fail, returns(0x%x).",(uint32_t)result);
    }

}
