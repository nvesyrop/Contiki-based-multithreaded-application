#include "contiki.h"
#include "sys/mt.h"
#include <stdio.h>
#define NUMBER_OF_URLS 3

char* service_urls[NUMBER_OF_URLS] = {"light","temper","humid"}; // 3 resources

static void
response_handler(coap_packet_t* response)
{
  uint16_t payload_len = 0;
  uint8_t* payload = NULL;
  payload_len = coap_get_payload(response, &payload);

  PRINTF("Response transaction id: %u", response->tid);
  if (payload) {
    memcpy(temp, payload, payload_len);
    temp[payload_len] = 0;
    PRINTF(" payload: %s\n", temp);
  }
}

static void
send_data(int s_id) // configured so that is can accept, as input, the id of the requested resource
{
  char buf[MAX_PAYLOAD_LEN];

  if (init_buffer(COAP_DATA_BUFF_SIZE)) {
    int data_size = 0;
    int service_id=s_id;
    coap_packet_t* request = (coap_packet_t*)allocate_buffer(sizeof(coap_packet_t));
    init_packet(request);

    coap_set_method(request, COAP_GET);
    request->tid = xact_id++;
    request->type = MESSAGE_TYPE_CON;
    coap_set_header_uri(request, service_urls[service_id]);

    data_size = serialize_packet(request, buf);

    PRINTF("Client sending request to:[");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF("]:%u/%s\n", (uint16_t)REMOTE_PORT, service_urls[service_id]);
    uip_udp_packet_send(client_conn, buf, data_size);

    delete_buffer();
  }
}

PROCESS(coap_client_example, "COAP Client Example");
AUTOSTART_PROCESSES(&coap_client_example);

PROCESS_THREAD(coap_client_example, ev, data)
{

void
 thread_func()
 { 
            for(;;){
            PRINTF("Incoming packet size: %u \n", (uint16_t)uip_datalen());
            if (uip_newdata()) {
            coap_packet_t* response = (coap_packet_t*)allocate_buffer(sizeof(coap_packet_t));
            if (response) {
            parse_message(response, uip_appdata, uip_datalen());
            response_handler(response);
            }
            }
            delete_buffer();
            mt_yield();
            }
}


static struct mt_thread light_thread; //declaration of the 3 threads
static struct mt_thread temper_thread;
static struct mt_thread humid_thread;

  PROCESS_BEGIN();

  mt_init();

 // definition of the threads + definition of the function that will be executed the first time each thread is being invoked + definition of the function’s inputs 

  mt_start(&light_thread, thread_func, NULL); 
  mt_start(&temper_thread, thread_func, NULL); 
  mt_start(&humid_thread, thread_func, NULL);


  SERVER_NODE(&server_ipaddr);

  /* new connection with server */
  client_conn = udp_new(&server_ipaddr, UIP_HTONS(REMOTE_PORT), NULL);
  udp_bind(client_conn, UIP_HTONS(LOCAL_PORT));

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
  UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

  etimer_set(&et, 5 * CLOCK_SECOND);

  while(1) {
    PROCESS_YIELD();
      int service_id;
    if (etimer_expired(&et)) {
      printf("time up!\n");
      service_id = random_rand() % NUMBER_OF_URLS;
      printf("service id is: %d\n",service_id);       
      send_data(service_id);    
      etimer_reset(&et);
    } else if (ev == tcpip_event) {
    if (init_buffer(COAP_DATA_BUFF_SIZE))
    {

// depending on the requested resource, the corresponding thread is being called

        switch(service_id){        
case 0: 
        	mt_exec(&light_thread);
        	break;   
        case 1: 
        	mt_exec(&temper_thread);
        	break;   
        case 2: 
        	mt_exec(&humid_thread);
        	break;
        default:
           	printf("No related resource\n");  
    }
     }
    }
  }

  mt_stop(&light_thread);
  mt_stop(&temper_thread);
  mt_stop(&humid_thread);
  mt_remove();

  PROCESS_END();
}
