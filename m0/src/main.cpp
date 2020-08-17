#include <atmel_start.h>
#include <stdio.h>
#include "SEGGER_RTT.h"

#define ID 0x45A
#define TYPE_SWITCH 0x00
volatile int counter = 0;

static uint32_t getMillis(void)
{
	return _calendar_get_counter(&CALENDAR_0.device);
}

volatile uint32_t prev_millis = 0;
static void button_on_PA28_pressed(void)
{
	uint8_t buttonport = 0;

	// TODO: configure hardware debounce
	const uint32_t current_millis = getMillis();
	uint32_t duration = current_millis - prev_millis;
	if (duration < 30)
		return; // debounce
	prev_millis = current_millis;
	struct can_message msg;
	uint8_t send_data[5];
	send_data[0] = buttonport;
	send_data[1] = TYPE_SWITCH;
	msg.id = ID;
	msg.type = CAN_TYPE_DATA;
	msg.data = send_data;
	msg.len = 5;
	msg.fmt = CAN_FMT_STDID;

	if (gpio_get_pin_level(BUTTON))
	{
		printf("rising edge detected RELEASED after %d ms \n", duration);
		send_data[2] = 0x00;
		send_data[3] = (duration & 0xFF);
		send_data[4] = (duration & 0xFF00) >> 8;
	}
	else
	{
		printf("falling edge detected PRESSED \n");
		send_data[2] = 0x01;
		send_data[3] = 0x00;
		send_data[4] = 0x00;
	}

	can_async_write(&CAN_0, &msg);
}

static void rotation(int step)
{
	printf("rotation step: %d\n", step);

	struct can_message msg;
	uint8_t send_data[4];
	send_data[0] = 0x00;
	send_data[1] = 0x00;
	send_data[2] = 0x00;
	send_data[3] = 0x00;

	msg.id = ID;
	msg.type = CAN_TYPE_DATA;
	msg.data = send_data;
	msg.len = 4;
	msg.fmt = CAN_FMT_STDID;

	msg.data[0] = counter & 0xff;
	msg.data[1] = (counter >> 8) & 0xff;

	can_async_write(&CAN_0, &msg);
}

int prev = 0; // 0 = default state, 1: A last non matched falling edge, 2: B
static void rotADetected()
{
	switch (prev)
	{
	case 0:
		prev = 1;
		break;
	case 1:
		break; // should not happen
	case 2:
		prev = 0;
		rotation(-1);
		break;
	}
}

static void rotBDetected()
{
	switch (prev)
	{
	case 0:
		prev = 2;
		break;
	case 1:
		prev = 0;
		rotation(1);
		break;
	case 2:
		break; // should not happen
	}
}

void CAN_0_tx_callback(struct can_async_descriptor *const descr)
{
	(void)descr;
	printf("Done CAN TX\n");
}
void CAN_0_rx_callback(struct can_async_descriptor *const descr)
{
	struct can_message msg;
	uint8_t data[64];
	msg.data = data;
	can_async_read(descr, &msg);
	printf("CAN RX: id: %#03x, len: %d, data: ", msg.id, msg.len);
	for (int i = 0; i < msg.len; i++)
		printf("%#02x ", msg.data[i]);
	printf("\n");
	return;
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	//  start calender: NOTE calender runs at 1,024 kHz => get date time won't be correct, but useful to get ms
	calendar_enable(&CALENDAR_0);

	ext_irq_register(PIN_PA28, button_on_PA28_pressed);
	ext_irq_register(PIN_PA14, rotBDetected);
	ext_irq_register(PIN_PA15, rotADetected);

	// struct io_descriptor *io;

	// usart_async_register_callback(&USART_0, USART_ASYNC_TXC_CB, tx_cb_USART_0);
	// /*usart_async_register_callback(&USART_0, USART_ASYNC_RXC_CB, rx_cb);
	// usart_async_register_callback(&USART_0, USART_ASYNC_ERROR_CB, err_cb);*/
	// usart_async_get_io_descriptor(&USART_0, &io);
	// usart_async_enable(&USART_0);

	printf("Hello World!\n");

	printf("Set CAN pin enable low\n");
	gpio_set_pin_level(CAN_EN, false);

	can_async_register_callback(&CAN_0, CAN_ASYNC_TX_CB, (FUNC_PTR)CAN_0_tx_callback);
	can_async_enable(&CAN_0);

	can_async_register_callback(&CAN_0, CAN_ASYNC_RX_CB, (FUNC_PTR)CAN_0_rx_callback);
	struct can_filter filter;
	filter.id = 0x42A;
	filter.mask = 0x1FFFFFFF;
	can_async_set_filter(&CAN_0, 0, CAN_FMT_STDID, &filter);

	while (1)
	{
		gpio_toggle_pin_level(LED15);
		// io_write(io, example_USART_0, 12);
		counter++;
		printf("Counter: %d \n", counter);

		delay_ms(1000);
	}
}
