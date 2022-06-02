#ifndef __CAN_CONTROLLER_H__
#define __CAN_CONTROLLER_H__

#include <atomic>
#include <cstdbool>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <thread>

#define MAX_CANBUS_DATA_LENGTH (8)

struct CanPacket
{
	uint32_t	m_id;
	uint8_t		m_dlc;
	uint8_t		m_data[MAX_CANBUS_DATA_LENGTH];
}__attribute__((packed));
typedef struct CanPacket CanPacket;

class CanController
{
public:
	class RxDelegate;

	CanController(std::shared_ptr<RxDelegate> rxDelegate);
	~CanController();

	bool send(CanPacket& packet);
	void start();
	void stop();

private:
	std::shared_ptr<RxDelegate>		m_rxDelagate;
	std::mutex						m_mutex;
	std::thread						m_rxThread;
	std::atomic_bool				m_isRunning;
};

class CanController::RxDelegate
{
public:
	virtual void onRxCanPacket(CanPacket& packet) = 0;
};

#endif
