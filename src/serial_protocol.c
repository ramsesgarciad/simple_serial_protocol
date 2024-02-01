#include "serial_protocol.h"
#include <stdio.h>
#include <string.h>

void serial_protocol_init(serial_protocol_t *const me)
{
    me->buffer_in.len = 0;
    me->buffer_out.len = 0;
}
static uint8_t serial_protocol_cs_calc(uint8_t *buffer, uint8_t len)
{
    uint32_t cs = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        cs += buffer[i];
    }
    uint8_t cs8 = cs % 256;
    printf("CS: %d, %02X\n", cs, cs8);
    return cs8;
}
static void serial_protocol_print_buffer(uint8_t *buffer, uint8_t len)
{
    printf("BUFFER[%d]:", len);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02X ", buffer[i]);
    }
    printf("END\n");
}
uint8_t serial_protocol_send_frame(serial_protocol_t *const me, uint8_t cmd, uint8_t *data, uint8_t len)
{

    if (len >= SERIAL_PROTOCOL_MAX_DATA_LEN)
    {
        return -1;
    }

    me->buffer_out.buffer[SERIAL_PROTOCOL_STX_POS] = SERIAL_PROTOCOL_STX;
    me->buffer_out.buffer[SERIAL_PROTOCOL_LEN_POS] = len;
    me->buffer_out.buffer[SERIAL_PROTOCOL_CMD_POS] = cmd;
    memcpy(me->buffer_out.buffer + SERIAL_PROTOCOL_HEADER_LEN, data, len);
    uint8_t i = SERIAL_PROTOCOL_HEADER_LEN + len;
    me->buffer_out.buffer[i++] = serial_protocol_cs_calc(me->buffer_out.buffer, i);
    serial_protocol_print_buffer(me->buffer_out.buffer, i);
    me->buffer_out.len = i;
    return 0;
}
uint8_t serial_protocol_parse_frame(serial_protocol_t *const me)
{

    if ((me->buffer_in.len > 0) || (me->buffer_in.buffer[SERIAL_PROTOCOL_STX_POS] == SERIAL_PROTOCOL_STX))
    {

        uint8_t inCsCalc = serial_protocol_cs_calc(me->buffer_in.buffer, me->buffer_in.len - 1);
        uint8_t inCs = me->buffer_in.buffer[me->buffer_in.len - 1];

        if (inCsCalc != inCs)
        {
            printf("CS error\n");
            me->buffer_in.len = 0;
            return -1;
        }

        uint8_t cmd = me->buffer_in.buffer[SERIAL_PROTOCOL_CMD_POS];
        for (uint8_t i = 0; i < me->handlers_index; i++)
        {
            printf("Handler %d\n", i);
            if (me->cmd_handlers[i].cmd == cmd)
            {
                printf("Handler found\n");
                serial_protocol_cmd_handler_t *handler = &me->cmd_handlers[i];
                handler->handler(me->buffer_in.buffer + SERIAL_PROTOCOL_HEADER_LEN, me->buffer_in.buffer[SERIAL_PROTOCOL_LEN_POS]);
                break;
            }
            else
            {
                printf("Handler not found\n");
            }
        }
    }
    else
    {
        printf("STX error or bad Len\n");
    }
}
void serial_protocol_add_handler(serial_protocol_t *const me, uint8_t cmd, void *handler)
{
    if (me->handlers_index < SERIAL_PROTOCOL_MAX_CMD)
    {
        me->cmd_handlers[me->handlers_index].cmd = cmd;
        me->cmd_handlers[me->handlers_index].handler = handler;
        me->handlers_index++;
    }
    else
    {
        printf("No more space for handlers\n");
    }
}
void serial_protocol_loop(serial_protocol_t *const me)
{
    if (me->buffer_out.len > 0)
    {
        printf("Sending %d bytes\n", me->buffer_out.len);
        me->write(me->buffer_out.buffer, me->buffer_out.len);
        me->buffer_out.len = 0;
    }

    if (me->available())
    {
        me->buffer_in.len = me->read(me->buffer_in.buffer, SERIAL_PROTOCOL_MAX_DATA_LEN);
        printf("Received %d bytes\n", me->buffer_in.len);
        serial_protocol_parse_frame(me);
    }
}
