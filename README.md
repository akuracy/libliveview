libliveview
===========

C library for communication with the Sony Ericsson LiveView.

Thanks to all work from:
  - https://github.com/dstenb/libliveview
  - https://code.google.com/p/openliveview/
  - https://code.google.com/p/adqmisc/source/browse/trunk/liveview
  
  
Added from the fork:

```c
int liveview_send_navigation(struct liveview *, unsigned char);
int liveview_send_vibrate(struct liveview *, int, int);
int liveview_send_status(struct liveview *);
int liveview_send_text(struct liveview *, const char *); // NO SEEM TO WORK
int liveview_send_image(struct liveview *, int, int, struct liveview_img *); // NO SEEM TO WORK
int liveview_send_clear(struct liveview *);
int liveview_send_alert(struct liveview *, int, int, int, const char *, const char *, const char *, struct liveview_img *);
int liveview_send_panel(struct liveview *, const char *, const char *, struct liveview_img *, uint8_t);
```

Still missing:
  - set led color
