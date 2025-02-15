#include <pgmspace.h>

#define credentials
#define THINGSNAME "esp32_uao"

const char* ssid = "";
const char* password = "";
const char AWS_IoT_Endpoint[] = "";


//Amazon Root CA 1

static const char AWS_CERT_CA[] PROGMEM = R"EOF(




)EOF";


//Device Certificate

static const char AWS_CERT_CRT[] PROGMEM = R"KEY(




)KEY";


//Device Private key

static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(




)KEY";