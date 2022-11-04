


/*
 * Functionalitate:
 * La primirea comenzii de la pardoseala se porneste pompa de caldura si pompa SC
 * Cand comanda de la pardoseala se opreste, se opreste si pompa de caldura, iar pompa SC mai merge 1 min
 * 
 * Implementare HW
 * intrare cu rezistor de 1k si diode de protectie. Pin configurat cu pullup
 * Placa releee. 1 releu pompa de caldura si unul pentru pompa SC
 * Cu Arduino nealimentat, cele 2 relee trebuie sa conduca
 * NTC-ul de ambient se inlocuieste cu un NTC pe iesirea din AT din SC si se inseriaza cu o rezistenta pt a limita temp
 * 
 * Implementare SW
 * Intrarea de la pardoseala configurata cu pullup
 * Clasa de intreruperi
 * Clasa de temporizari
 * Clasa de stari
 * 
 * Testare
 * Test "rapid". Fortat
 * Thingspeak
 * 
 * 
 * 
 * 
 * 
 * Daca dupa x (5 minute) de la pornirea pompei de caldura
 * nu apare diferenta de temperatura pe refrigerant, 
 * se opreste pompa de caldura si se porneste centrala. Se va activa un cod de alerta
 * 
 * Pompa SC va porni daca exista un prag de diferenta de temperatura (de verificat debitul si sa existe COP)
 * si se va opri la x minute dupa ce diferenta scade sub alt prag.
 * 
 * 
 * Daca in functionare apar 3 caderi de consum
 * 
 * 
 */
