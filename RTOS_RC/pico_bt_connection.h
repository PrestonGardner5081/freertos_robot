#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 1000

typedef struct CommandState{
    bool w;
    bool a;
    bool s;
    bool d;
};

struct CommandState bt_connection_get_internal_command_state();

void spp_service_setup(void);

