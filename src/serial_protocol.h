#ifndef __SERIALPROTOCOL_H__
#define __SERIALPROTOCOL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SERIAL_PROTOCOL_STX 0x02
#define SERIAL_PROTOCOL_STX_POS 0
#define SERIAL_PROTOCOL_LEN_POS 1
#define SERIAL_PROTOCOL_CMD_POS 2
#define SERIAL_PROTOCOL_MAX_CMD 4

#define SERIAL_PROTOCOL_HEADER_LEN 3
#define SERIAL_PROTOCOL_MAX_DATA_LEN (255 - SERIAL_PROTOCOL_HEADER_LEN)

    /*
     * @brief Serial Protocol Handler
     * @param data The data
     * @param len The length of the data
     */
    typedef void (*serial_protocol_handler_t)(uint8_t *data, uint8_t len);

    /*
     * @brief Serial Protocol Write
     * @param data The data
     * @param len The length of the data
     * @return The number of bytes written
     */

    typedef uint8_t (*serial_protocol_write)(uint8_t *data, uint8_t len);

    /*
     * @brief Serial Protocol Read
     * @param data The data
     * @param len The length of the data
     * @return The number of bytes read
     */
    typedef uint8_t (*serial_protocol_read)(uint8_t *data, uint8_t len);

    /*
     * @brief Serial Protocol Available
     * @return 1 if available, 0 if not
     */
    typedef uint8_t (*serial_protocol_available)(void);

    /*
     * @brief Serial Protocol Commands
     */

    typedef enum serial_protocol_cmds
    {

        CMD_GET_TEMPERATURE = 0xA0, /**< Get Temperature */
        CMD_GET_HUMIDITY = 0xA1,    /**< Get Humidity */
        CMD_GET_PRESSURE = 0xA2,    /**< Get Pressure */
        CMD_SET_LED = 0xA3,         /**< Set LED */
    } serial_protocol_cmds_e;

    typedef struct serial_protocol_buffer
    {
        uint8_t buffer[256];
        uint8_t len;
    } serial_protocol_buffer_t;

    /*
     * @brief Serial Protocol Command Handler
     */
    typedef struct serial_protocol_cmd_handler
    {
        serial_protocol_cmds_e cmd;        /**< Command */
        serial_protocol_handler_t handler; /**< Handler */
    } serial_protocol_cmd_handler_t;       /**< Command Handler */

    /*
     * @brief Serial Protocol
     */
    typedef struct serial_protocol
    {
        serial_protocol_write write;                                         /**< Handler for write */
        serial_protocol_read read;                                           /**< Handler for read */
        serial_protocol_available available;                                 /**< Handler for available */
        serial_protocol_buffer_t buffer_in;                                  /**< Buffer in */
        serial_protocol_buffer_t buffer_out;                                 /**< Buffer out */
        serial_protocol_cmd_handler_t cmd_handlers[SERIAL_PROTOCOL_MAX_CMD]; /**< Command handlers */
        uint8_t handlers_index;                                              /**< Handlers index */

    } serial_protocol_t;

    /*
     * @brief Initialize the serial protocol
     * @param me The serial protocol object
     */
    void serial_protocol_init(serial_protocol_t *const me);

    /*
     * @brief Send a frame
     * @param me The serial protocol object
     * @param cmd The command
     * @param data The data
     * @param len The length of the data
     * @return 0 if success, 1 if error
     */

    uint8_t serial_protocol_send_frame(serial_protocol_t *const me, uint8_t cmd, uint8_t *data, uint8_t len);

    /*
     * @brief Parse a frame
     * @param me The serial protocol object
     * @return 0 if success, 1 if error
     */

    uint8_t serial_protocol_parse_frame(serial_protocol_t *const me);

    /*
     * @brief Get the buffer out
     * @param me The serial protocol object
     * @return The buffer out
     */

    serial_protocol_buffer_t serial_protocol_get_buffer_out(serial_protocol_t *const me);

    /*
     * @brief Clear the buffer out
     * @param me The serial protocol object
     */
    void serial_protocol_clear_buffer_out(serial_protocol_t *const me);

    /*
     * @brief Add a handler
     * @param me The serial protocol object
     * @param cmd The command
     * @param handler The handler
     */

    void serial_protocol_add_handler(serial_protocol_t *const me, uint8_t cmd, void *handler);

    /*
     * @brief Get the buffer in
     * @param me The serial protocol object
     * @return The buffer in
     */

    serial_protocol_buffer_t *serial_protocol_get_buffer_in(serial_protocol_t *const me);

    /*
     * @brief Loop Service
     * @param me The serial protocol object
     */

    void serial_protocol_loop(serial_protocol_t *const me);

#ifdef __cplusplus
}
#endif

#endif // __SERIALPROTOCOL_H__
