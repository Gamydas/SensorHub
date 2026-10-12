
#include <include/lwip/err.h>
#include <include/lwip/sys.h>
#include <lwip/netif.h>
#include <projdefs.h>
#include "FreeRTOS.h"
#include "task.h"
#include "xil_printf.h"
#include "xttcps.h"
#include "lwip/init.h"
#include "xparameters.h"
#include "lwip/dhcp.h"
#include "netif/xadapter.h"
#include "lwip/apps/httpd.h"
#include "lwip/tcpip.h"

extern XTtcPs_Config XTtcPs_ConfigTable[];
// this is the network interface of lwip, needs to be global and exist throughout the entire runtime
static struct netif server_netif;

#define THREAD_STACKSIZE 1024


void test(void *param)
{
    (void)param;
    while(1)
    {
        xil_printf("Hello World!\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void httpd_init_wrapper(void *ctx)
{
	(void)ctx;
	httpd_init();	
}

void network_init_task(void *param)
{
    (void)param;
    lwip_init();
    // strucutre to initialize network interface 
    struct netif *netif;
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
    // declare address, netmask and gateway
    ip_addr_t ipaddr, netmask, gw;
    netif = &server_netif;
    // initialize IP addresses to be used
    IP4_ADDR(&ipaddr,  0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw,      0, 0, 0, 0);

    // initialized ethernet controller of the board and sets mac address and start IPs, also adds netif to the lwip netif list
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, XPAR_XEMACPS_0_BASEADDR)) 
    {
	    xil_printf("Error adding N/W interface\r\n");
	    vTaskDelete(NULL);
    }
    
    // sets interface to be default route
    netif_set_default(netif);
    // starts interface
    netif_set_up(netif);
    // start address distribution   
    dhcp_start(netif);

    // start packet receive thread - required for lwIP operation 
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);


    while(netif->ip_addr.addr == 0)
    {
        xil_printf("Waiting for IPs\r\n");
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    
    // print board ip settings
    xil_printf("IP: %s\r\n", ip4addr_ntoa(&netif->ip_addr));
    xil_printf("MASK: %s\r\n", ip4addr_ntoa(&netif->netmask));
    xil_printf("GW: %s\r\n", ip4addr_ntoa(&netif->gw));
    
    // stats httpd_init in the tcpip thread
    if (tcpip_callback(httpd_init_wrapper, NULL) != ERR_OK)
    {
        xil_printf("ERROR TRYING TO START FUNCTION IN TCPIP THREAD!\r\n");
    }
    // interface has been initialized succesfully, task gets deleted 
    vTaskDelete(NULL);
}


int main()
{
    xil_printf("Hello World!\r\n");
    // xil_printf("TTC0: %x\r\n", XTtcPs_ConfigTable[0].BaseAddress);
    // creates the lwip initializer Task with the above defined function test
    // params: function, name, size in WORDS(1 word on this device is 4 byte), parameters so NULL for this, priority is one higher than the idle task, Reference to interact
    if (sys_thread_new("LWIP THRD", network_init_task, NULL, 4*THREAD_STACKSIZE, DEFAULT_THREAD_PRIO) == NULL)
    {
        xil_printf("ERROR CREATING TASK. EXITING.\r\n");
        while(1);
    }
    
    vTaskStartScheduler();
    xil_printf("ERROR STARTING SCHEDULER. EXITING.\r\n");
    while(1);
}