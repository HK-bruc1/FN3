#include "system/includes.h"
/* #include "clock_manager/clock_manager.h" */
#include "multi_protocol_main.h"
#include "bt_tws.h"

#if SMART_BOX_EN

/* #if (BT_AI_SEL_PROTOCOL & (RCSP_MODE_EN | GFPS_EN)) */

#define ATT_LOCAL_PAYLOAD_SIZE    (517)//(517)              //note: need >= 20
#define ATT_SEND_CBUF_SIZE        (512*2)                   //note: need >= 20,缓存大小，可修改
#define ATT_RAM_BUFSIZE           (ATT_CTRL_BLOCK_SIZE + ATT_LOCAL_PAYLOAD_SIZE + ATT_SEND_CBUF_SIZE)                   //note:
static u8 att_ram_buffer[ATT_RAM_BUFSIZE] __attribute__((aligned(4)));

typedef struct {
    // linked list - assert: first field
    void *offset_item;
    // data is contained in same memory
    u32        service_record_handle;
    u8         *service_record;
} sdp_protocal_item_t;
#define SDP_RECORD_REGISTER(handler) \
	const sdp_protocal_item_t  handler sec(.sdp_record_item)

bool check_tws_master_role()
{
#if TCFG_USER_TWS_ENABLE
    return (tws_api_get_role() == TWS_ROLE_MASTER);
#endif
    return 1;
}

void ble_profile_init(void)
{
    /* clock_alloc("m_protocol", 12 * 1000000L); */
    le_device_db_init();
    /* app_ble_sm_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING, 7, 0); */
    /* app_ble_sm_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_BONDING, 7, 0); */
#if TCFG_BLE_BRIDGE_EDR_ENALBE
    ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING, 7, 0);
#else
    ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING, 7, 0);
#endif
    app_ble_init();
    app_spp_init();

    multi_protocol_common_callback_init();

    ble_op_multi_att_send_init(att_ram_buffer, ATT_RAM_BUFSIZE, ATT_LOCAL_PAYLOAD_SIZE);

#if SMART_BOX_EN
    extern void le_smartbox_ble_profile_init(void);
    le_smartbox_ble_profile_init();
#endif
}

void bt_ble_init(void)
{

#if SMART_BOX_EN
    extern void le_smartbox_bt_ble_init(void);
    le_smartbox_bt_ble_init();
#endif

    extern void ancs_ams_set_ios_pair_request_enable(u8 enable);
    ancs_ams_set_ios_pair_request_enable(0);

#if FINDMY_EN
    extern void fmy_bt_ble_init(void);
    fmy_bt_ble_init();
#endif /* #if FINDMY_EN */
}

void bt_ble_exit(void)
{
#if SMART_BOX_EN
    extern void le_smartbox_bt_ble_exit(void);
    le_smartbox_bt_ble_exit();
#endif

#if FINDMY_EN
    extern void fmy_bt_ble_exit(void);
    fmy_bt_ble_exit();
#endif /* #if FINDMY_EN */
    multi_protocol_common_callback_exit();

    app_ble_exit();
    app_spp_exit();
}

void bt_ble_adv_enable(u8 enable)
{
#if SMART_BOX_EN
    extern void le_smartbox_bt_ble_adv_enable(u8 enable);
    le_smartbox_bt_ble_adv_enable(enable);
#endif

}

#endif
