#ifndef __OTA_INCLUDED__
#define __OTA_INCLUDED__

#define OTA_SETUP_IF_ENABLED ota_setup()
#define OTA_LOOP_IF_ENABLED ota_loop()

void ota_setup();
void ota_loop();
void ota_setup_and_wait();

#endif
