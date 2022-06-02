/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#include "CanController.h"

class CanRxHandler
: public CanController::RxDelegate
{
public:
	void onRxCanPacket(CanPacket& packet) override
	{
		printf("CanRxHandler::Received CAN Packet\n");
		printf("CanRxHandler:: %08X %02X ", packet.m_id, packet.m_dlc);
		for (uint8_t i = 0; i < packet.m_dlc; ++i)
			printf("%02X ", packet.m_data[i]);
		printf("\n");
	}
};

extern "C" {
	void app_main(void)
	{
		auto rxHandler = std::make_shared<CanRxHandler>();

		CanController controller(rxHandler);

		controller.start();

		while (1)
		{
			printf("[Main Loop]\n");
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
	}
}
