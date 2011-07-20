// Definitions used in both server and client

#define NUM_POWER_VALUES 8
#define NUM_TIME_SLOTS 5

#define INTERNET_DOMAIN 0
#if INTERNET_DOMAIN
  #define HOST_NAME   "localhost"
  #define PORTNO      20000
#else // UNIX DOMAIN
  #define SOCKET_NAME "3d-ice.socket"
#endif

