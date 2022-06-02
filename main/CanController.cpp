#include "CanController.h"

#include "driver/gpio.h"
#include "driver/can.h"
#include "esp_log.h"

#include <chrono>
#include <exception>


CanController::CanController(std::shared_ptr<RxDelegate> rxDelegate)
: m_rxDelagate(rxDelegate)
{
	//Initialize configuration structures using macro initializers
	can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, CAN_MODE_NORMAL);
	can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
	can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

	//Install CAN driver
	if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
		ESP_LOGI("CanController", "Driver installed\n");
	else
		ESP_LOGI("CanController", "Failed to install driver\n");; // throw std::runtime_error("Failed to install driver\n");

	//Start CAN driver
	if (can_start() == ESP_OK)
		ESP_LOGI("CanController", "Driver started\n");
	else
		ESP_LOGI("CanController", "Failed to start driver\n"); // throw std::runtime_error("Failed to start driver\n");
}

CanController::~CanController()
{
	stop();
}

bool CanController::send(CanPacket& packet)
{
	bool result = false;

	//Configure message to transmit
	can_message_t message;
	message.identifier = packet.m_id;
	message.flags = TWAI_MSG_FLAG_NONE;
	message.data_length_code = packet.m_dlc;
	for (int i = 0; i < message.data_length_code; i++)
		message.data[i] = packet.m_data[i];

	//Queue message for transmission
	if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
		ESP_LOGI("CanController", "Message queued for transmission\n");
	else
		ESP_LOGI("CanController", "Failed to queue message for transmission\n");; //throw std::runtime_error("Failed to queue message for transmission\n");

	return result;
}

void CanController::start()
{
	if (m_isRunning.load())
		return;

	m_rxThread = std::thread([this]() {
		m_isRunning.store(true);

		while (1)
		{
			// CAN Receive Loop goes here

			//Wait for message to be received
			can_message_t message;
			if (can_receive(&message, pdMS_TO_TICKS(1000)) != ESP_OK)
				continue;

			//Process received message
			if (m_rxDelagate)
			{
				CanPacket packet;

				packet.m_id = message.identifier;
				packet.m_dlc = message.data_length_code;
				for (int i = 0; i < message.data_length_code; i++)
					packet.m_data[i] = message.data[i];

				m_rxDelagate->onRxCanPacket(packet);
			}
		}

		m_isRunning.store(false);
	});
}

void CanController::stop()
{
	if (m_rxThread.joinable())
		m_rxThread.join();
}
