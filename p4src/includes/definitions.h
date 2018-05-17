/**************************
 * Definitions
**************************/
#define PACKET_LORA 1

/* Lora Fctrol downlink */
#define LORA_FCTRL_DOWN_ADR      0x80
#define LORA_FCTRL_DOWN_RFU      0x40
#define LORA_FCTRL_DOWN_ACK      0x20
#define LORA_FCTRL_DOWN_FPENDING 0x10
#define LORA_FCTRL_DOWN_FOPTS    0x0F

/* Lora Fctrol uplink  */
#define LORA_FCTRL_UP_ADR       0x80
#define LORA_FCTRL_UP_ADRACKREQ 0x40
#define LORA_FCTRL_UP_DOWN_ACK  0x20
#define LORA_FCTRL_UP_RFU       0x10
#define LORA_FCTRL_UP_FOPTS     0x0F

/* Pkt type */
#define TABLE_FORWARD_ID        1
#define TABLE_CHANNEL_ID        2
#define TABLE_RX_DELAY          3

/* Mirror Session  */
#define CPU_MIRROR_SESSION_ID   250

/* Technologies  */
#define TECHNOLOGY_LORA         1
#define TECHNOLOGY_NBIOT        2

/* Swicht Port  */
#define SWITCH_PORT_1           1
#define SWITCH_PORT_1           2
#define SWITCH_PORT_CONTROLLER  3

/* Packet type  */
#define PKT_UPLINK              1
#define PKT_DOWNLINK            2

/* Miselanious  */
#define CPU_MIRROR_SESSION_ID  250
