#include "libliveview.h"

sdp_session_t *register_service()
{
	uint32_t service_uuid_int[] = { 0, 0, 0, 0xC0FE };
	uint8_t rfcomm_channel = 1;
	sdp_record_t rec;
	sdp_session_t *session = NULL;
	uuid_t root_uuid, rfcomm_uuid, svc_uuid, l2cap_uuid;
	sdp_data_t *channel = NULL;
	sdp_list_t *rfcomm_list = NULL, *root_list = NULL, *l2cap_list = NULL,
		   *proto_list = NULL, *access_proto_list = NULL,
		   *profile_list = NULL;
	sdp_profile_desc_t profile;

	memset(&profile, 0, sizeof(profile));
	memset(&rec, 0, sizeof(rec));

	sdp_uuid128_create(&svc_uuid, &service_uuid_int);
	sdp_set_service_id(&rec, svc_uuid);

	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root_list = sdp_list_append(NULL, &root_uuid);
	sdp_set_browse_groups(&rec, root_list);

	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	l2cap_list = sdp_list_append(NULL, &l2cap_uuid);
	proto_list = sdp_list_append(NULL, l2cap_list);

	sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
	profile.version = 0x0100;
	profile_list = sdp_list_append(0, &profile);
	sdp_set_profile_descs(&rec, profile_list);

	sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
	channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
	rfcomm_list = sdp_list_append(NULL, &rfcomm_uuid);
	sdp_list_append(rfcomm_list, channel);
	sdp_list_append(proto_list, rfcomm_list);

	access_proto_list = sdp_list_append(NULL, proto_list);
	sdp_set_access_protos(&rec, access_proto_list);

	sdp_set_info_attr(&rec, "LiveView", "", "");

	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
	sdp_record_register(session, &rec, 0);

	sdp_data_free(channel);
	sdp_list_free(l2cap_list, NULL);
	sdp_list_free(rfcomm_list, NULL);
	sdp_list_free(root_list, NULL);
	sdp_list_free(access_proto_list, NULL);
	sdp_list_free(profile_list, NULL);

	return session;
}

int liveview_init(struct liveview *lv)
{
	struct sockaddr_rc addr;

	if ((lv->listen_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
		return -1;

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_bdaddr = *BDADDR_ANY;
	addr.rc_channel = 1;

	if (bind(lv->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(lv->fd);
		return -1;
	}

	listen(lv->listen_fd, 1);

	lv->session = register_service();

	return 0;
}

int liveview_connect(struct liveview *lv)
{
	struct sockaddr_rc addr;
	socklen_t opt = sizeof(struct sockaddr_rc);

	lv->fd = accept(lv->listen_fd, (struct sockaddr *)&addr, &opt);
	sdp_close(lv->session);

	return lv->fd;
}
