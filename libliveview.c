#include "libliveview.h"

static void send_msg(struct liveview *, struct liveview_msg *);
static int send_wrapper(struct liveview *, struct liveview_msg *);
static sdp_session_t *register_service(void);

/* "private" functions, not to be declared in libliveview.h */
void debug_msg(struct liveview_msg *);
int liveview_msg_read(struct liveview *, struct liveview_msg *);
int liveview_fill_event(struct liveview_event *, struct liveview_msg *);

void send_msg(struct liveview *lv, struct liveview_msg *msg)
{
	if (lv->fd != -1 && msg) {
		char header[6];

		header[0] = msg->id;
		header[1] = 4;
		header[2] = (msg->payload_len >> 24) & 0xFF;
		header[3] = (msg->payload_len >> 16) & 0xFF;
		header[4] = (msg->payload_len >> 8) & 0xFF;
		header[5] = (msg->payload_len) & 0xFF;

		write(lv->fd, header, 6);
		write(lv->fd, msg->payload, msg->payload_len);

		printf("=> %d\n", msg->id);
	}
}

static int send_wrapper(struct liveview *lv, struct liveview_msg *msg)
{
	if (!msg)
		return -1;
	send_msg(lv, msg);
	msg_free(msg);

	return 0;
}

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

	if ((lv->listen_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0) {
		return -1;
	}

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

int liveview_msg_read(struct liveview *lv, struct liveview_msg *msg)
{
	uint32_t payload_read = 0;
	char buf[4];
	char *p;

	if (read(lv->fd, &msg->id, 1) < 0)
		return -1;
	if (read(lv->fd, &msg->header_len, 1) < 0)
		return -1;
	if (read(lv->fd, buf, 4) < 0)
		return -1;

	msg->payload_len = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];

	if (!(msg->payload = calloc(msg->payload_len, sizeof(char))))
		return -1;

	p = msg->payload;
	while (msg->payload_len > payload_read) {
		int32_t tmp = read(lv->fd, p, msg->payload_len - payload_read);

		if (tmp < 0)
			return -1;
		payload_read += (uint32_t)tmp;
		p += tmp;
	}

	return 0;
}

void debug_msg(struct liveview_msg *msg)
{
	uint32_t i;
	printf("id: %i, header: %i, payload: %i\n", msg->id, msg->header_len, msg->payload_len);
	for (i = 0; i < msg->payload_len; i++) {
		printf("[%d] 0x%2.2X\n", i, *(msg->payload + i));
	}
}

int liveview_fill_event(struct liveview_event *ev, struct liveview_msg *msg)
{
	uint8_t navigation;

	if (ev->type == M_GETALERT) {
		ev->menu_item_id = msg->payload[0];
		ev->alert_action = msg->payload[1];
		/* ev->max_body_size = msg->payload[2]; // TODO: ?? */
		debug_msg(msg);
	} else if (ev->type == M_NAVIGATION) {
		/* msg->payload[0]; // should be 0 */
		/* msg->payload[1]; // should be 3 */
		navigation = msg->payload[2];
		ev->menu_item_id = msg->payload[3];
		ev->menu_id = msg->payload[4];
		ev->in_alert = ev->menu_id == 20;

		if (navigation != 32 && ((navigation < 1) || (navigation > 15))) {
			/* out of range */
		} else {
			if (navigation == 32) {
				ev->nav_action = NAVACTION_PRESS;
				ev->nav_type   = NAVTYPE_MENUSELECT;
			} else {
				ev->nav_action = (navigation - 1) % 3;
				ev->nav_type   = (navigation - 1) / 3;
			}
		}
		debug_msg(msg);
	}

	return 0;
}

int liveview_read(struct liveview *lv, struct liveview_event *ev)
{
	int ret;
	struct liveview_msg *msg;

	if (!(msg = calloc(1, sizeof(struct liveview_msg))))
		return -1;

	if (liveview_msg_read(lv, msg) < 0) {
		msg_free(msg);
		return -1;
	}

	ev->type = msg->id;

	ret = liveview_fill_event(ev, msg);

	msg_free(msg);

	return ret;
}

int liveview_send_display_properties_request(struct liveview *lv)
{
	return send_wrapper(lv, msg_create(M_DISPLAY_PROPERTIES_REQUEST, "s",
			SW_VERSION));
}

int liveview_send_ack(struct liveview *lv, char id)
{
	printf("send ack for event %d\n", id);
	return send_wrapper(lv, msg_create(M_ACK, "b", id));
}

int liveview_send_menu_item(struct liveview *lv, int id, int alert,
		int unread, const char *text, struct liveview_img *img)
{
	struct liveview_msg *msg;
	
	msg = msg_create(M_GETMENUITEM_RESP, "bhhhbbhhhsi",
				!alert, 0, unread, 0, id + 3, 0, 0, 0,
				strlen(text), text, img);
	/* img_free(img); // done by img_create() caller */

	return send_wrapper(lv, msg);
}

int liveview_send_menu_settings(struct liveview *lv, uint8_t vtime, uint8_t id)
{
	return send_wrapper(lv, msg_create(M_SETMENUSETTINGS, "bbb", vtime, 12, 
				id));
}

int liveview_send_menu_size(struct liveview *lv, unsigned char size)
{
	return send_wrapper(lv, msg_create(M_SETMENUSIZE, "b", size));
}

int liveview_send_time(struct liveview *lv, uint32_t time, uint8_t h24)
{
	return send_wrapper(lv, msg_create(M_GETTIME_RESP, "lb", time, !h24));
}

int liveview_send_navigation(struct liveview *lv, unsigned char result)
{
	return send_wrapper(lv, msg_create(M_NAVIGATION_RESP, "b", result));
}

int liveview_send_vibrate(struct liveview *lv, int delay, int on)
{
	return send_wrapper(lv, msg_create(M_SETVIBRATE, "hh", delay, on));
}

int liveview_send_status(struct liveview *lv)
{
	return send_wrapper(lv, msg_create(M_DEVICESTATE_ACK, "b", RESULT_OK));
}

int liveview_send_text(struct liveview *lv, const char *text)
{
	return send_wrapper(lv, msg_create(M_DISPLAYTEXT, "bhs", 0, strlen(text), text));
}

int liveview_send_image(struct liveview *lv, int x, int y, struct liveview_img *img)
{
	struct liveview_msg *msg;

	msg = msg_create(M_DISPLAYBITMAP, "bbbi", x, y, 1, img);
	/* img_free(img); // done by img_create() caller */

	return send_wrapper(lv, msg);
}

int liveview_send_clear(struct liveview *lv)
{
	return send_wrapper(lv, msg_create(M_CLEARDISPLAY, "b", 0));
}

int liveview_send_alert(struct liveview *lv, int alert_index, int total_count, int unread_count, const char * timestamp, const char *header, const char *body, struct liveview_img *img)
{
	struct liveview_msg *msg;

	msg = msg_create(M_GETALERT_RESP, "bhhhbbhshshsbbbhi", 0, total_count, unread_count, alert_index, 0, 0, strlen(timestamp), timestamp, strlen(header), header, strlen(body), body, 0, 0, 0, img->length, img);
	/* img_free(img); // done by img_create() caller */

	return send_wrapper(lv, msg);
}

int liveview_send_panel(struct liveview *lv, const char *header, const char *footer, struct liveview_img *img, uint8_t alert_user)
{
	struct liveview_msg *msg;

	msg = msg_create(M_DISPLAYPANEL, "bhhhbbhshshsi", 0, 0, 0, 0, alert_user ? 80 : 81, 0, strlen(header), header, 0, "", strlen(footer), footer, img);
	/* img_free(img); // done by img_create() caller */

	return send_wrapper(lv, msg);
}

int liveview_send_screenmode(struct liveview *lv, uint8_t brightness, uint8_t automatic)
{
	brightness = brightness << 1;
	if (automatic) {
		brightness |= 1;
	}
	
	return send_wrapper(lv, msg_create(M_SETSCREENMODE, "b", brightness));
}
