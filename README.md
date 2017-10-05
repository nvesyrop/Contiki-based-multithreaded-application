# Contiki-based-multithreaded-application
Using 3 threads for handling 3 resources in the Contiki OS.

Based on the Coap-rest client example, but also utilizing the existing optional mt library of Contiki OS.

This is a simple application that can be employed in the Cooja enviroment. 
It enables the definition and handling of 3 threads that have access to 3 different resources,
through RESTful WS calls (using the CoAP protocol).
