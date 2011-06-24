// Definitions used in both server and client

#define DYNAMIC_POWER_VALUES 1
#define INTERNET_DOMAIN      0

#if INTERNET_DOMAIN
  #define HOST_NAME   "localhost"
  #define PORTNO      20000
#else
  #define SOCKET_NAME "3d-ice.socket"
#endif

