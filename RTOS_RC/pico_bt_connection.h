#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 1000

typedef struct CommandState{
    uint16_t xPercent;
    uint16_t yPercent;
};

struct CommandState bt_connection_get_internal_command_state();

void spp_service_setup(void);

void bt_string_transmit(char* send_str);

