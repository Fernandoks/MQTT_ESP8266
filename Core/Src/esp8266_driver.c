/*
 * esp8266_driver.c
 *
 *  Created on: Apr 3, 2020
 *      Author: fernandoks
 */


#include "esp8266_driver.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define pc_uart 	&huart2
#define esp_uart 	&huart1
#define ESP_Queue 	UART1_Queue


extern osMessageQId UART1_Queue;
extern osMessageQId UART2_Queue;

uint8_t *espAT = "AT";
uint8_t *espVersion = "AT+GMR";
uint8_t *espATMode = "AT+CWMODE=2";

uint8_t* name="<p>Circuit Digest</p><p>A community of electrical and electronics students, engineers and makers</p>";   //String with html notations
uint8_t* data="<p>Data Received Successfully.....</p>";     //String with html
uint8_t *Basic_inclusion = "<!DOCTYPE html> <html>\n<head><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
		<title>LED CONTROL</title>\n<style>html { font-family: Helvetica; \
		display: inline-block; margin: 0px auto; text-align: center;}\n\
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
		width: 80px;background-color: #1abc9c;border: none;color: white;\
		padding: 13px 30px;text-decoration: none;font-size: 25px;\
		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
		.button-on {background-color: #1abc9c;}\n.button-on:active \
		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
		</style>\n</head>\n<body>\n<h1>FKS - F446RE - ESP8266 </h1>\n";

uint8_t *LED_ON = "<p>LED Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>";
uint8_t *LED_OFF = "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>";
uint8_t *Terminate = "</body></html>";


char buffer[20];

/*****************************************************************************************************************************************
 * Send command to ESP_queue
 ****************************************************************************************************************************************/

ESP8266_t esp_command(unsigned char* command, uint32_t command_size)
{
	for (int i = 0; i < command_size; ++i)
	{
		if(osMessagePut (ESP_Queue, command[i], 1) != osOK)
		{
		  return ESP8266_ERROR;
		}

	}

	return ESP8266_SUCCESS;
}




#if 0


/*****************************************************************************************************************************************
 * ESP Init
 ****************************************************************************************************************************************/

void ESP_Init (char *SSID, char *PASSWD)
{
	char data[80];


	esp_command("AT+RST\r\n", wifi_uart);
	Uart_sendstring("RESETTING.", pc_uart);
	for (int i=0; i<5; i++)
	{
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}

	/********* AT **********/
	Uart_sendstring("AT\r\n", wifi_uart);
	while(!(Wait_for("AT\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("AT---->OK\n\n", pc_uart);


	/********* AT+CWMODE=1 **********/
	Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
	while (!(Wait_for("AT+CWMODE=1\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("CW MODE---->1\n\n", pc_uart);


	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_sendstring("connecting... to the provided AP\n", pc_uart);
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("WIFI GOT IP\r\n\r\nOK\r\n", wifi_uart)));
	sprintf (data, "Connected to,\"%s\"\n\n", SSID);
	Uart_sendstring(data,pc_uart);


	/********* AT+CIFSR **********/
	Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
	while (!(Wait_for("CIFSR:STAIP,\"", wifi_uart)));
	while (!(Copy_upto("\"",buffer, wifi_uart)));
	while (!(Wait_for("OK\r\n", wifi_uart)));
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	sprintf (data, "IP ADDR: %s\n\n", buffer);
	Uart_sendstring(data, pc_uart);


	Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
	while (!(Wait_for("AT+CIPMUX=1\r\r\n\r\nOK\r\n", wifi_uart)));
	Uart_sendstring("CIPMUX---->OK\n\n", pc_uart);

	Uart_sendstring("AT+CIPSERVER=1,80\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("CIPSERVER---->OK\n\n", pc_uart);

	Uart_sendstring("Now Connect to the IP ADRESS\n\n", pc_uart);

}




int Server_Send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for(">", wifi_uart)));
	Uart_sendstring (str, wifi_uart);
	while (!(Wait_for("SEND OK", wifi_uart)));
	sprintf (data, "AT+CIPCLOSE=5\r\n");
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	return 1;
}

void Server_Handle (char *str, int Link_ID)
{
	char datatosend[1024] = {0};
	if (!(strcmp (str, "/ledon")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_ON);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/ledoff")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

}

void Server_Start (void)
{
	char buftocopyinto[64] = {0};
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID, wifi_uart)));
	Link_ID -= 48;
	while (!(Copy_upto(" HTTP/1.1", buftocopyinto, wifi_uart)));
	if (Look_for("/ledon", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
		Server_Handle("/ledon",Link_ID);
	}

	else if (Look_for("/ledoff", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		Server_Handle("/ledoff",Link_ID);
	}

	else if (Look_for("/favicon.ico", buftocopyinto) == 1);

	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		Server_Handle("/ ", Link_ID);
	}
}

#endif

/*

void esp_checkIP(uint32_t t1)                                     //A function to check ip of ESP8266
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(Serial2.available()>0)
    {
      if(Serial2.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}

void esp_get_ip()                                           //After cheacking ip ,this is a function to get IP address
{
  IP="";
  char ch=0;
  while(1)
  {
    Serial2.printlnprintln("AT+CIFSR");                   //GET IP AT COMMAND
    while(Serial2.available()>0)
    {
      if(Serial2.find("STAIP,"))                   //This finds the STAIP that is the STATIC IP ADDRESS of ESP8266
      {
        delay(1000);
        Serial.print("IP Address:");
        while(Serial2.available()>0)
        {
          ch=Serial2.read();                      //Serial2 reads from ESP8266
          if(ch=='+')
          break;
          IP+=ch;
        }
      }
      if(ch=='+')
      break;
    }
    if(ch=='+')
    break;
    delay(1000);
  }
  Serial.print(IP);                                //prints IP address in Serial monitor
  Serial.print("Port:");
  Serial.println(80);
}

void esp_connect(char* cmd, int t)                  //This function is for connecting ESP8266 with wifi network by using AT commands
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);                  //Sends to serial monitor
    Serial2.println(cmd);                 //sends to ESP8266 via serial communication
    while(Serial2.available())
    {
      if(Serial2.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  Serial.println("OK");
  else
  Serial.println("Error");
}

void wifi_init()                                //This function contains AT commands that passes to connect_wifi()
{
      connect_wifi("AT",100);                   //Sends AT command with time(Command for Achknowledgement)
      connect_wifi("AT+CWMODE=3",100);          //Sends AT command with time (For setting mode of Wifi)
      connect_wifi("AT+CWQAP",100);            //Sends AT command with time (for Quit AP)
      connect_wifi("AT+RST",5000);             //Sends AT command with time (For RESETTING WIFI)
      check4IP(5000);
      if(!No_IP)
      {

        Serial.println("Connecting Wifi....");
        connect_wifi("AT+CWJAP=\"Pramo\",\"pokemon08\"",7000);         //provide your WiFi username and password here

      }
      else
        {
        }
      Serial.println("Wifi Connected");
      get_ip();

      connect_wifi("AT+CIPMUX=1",100);                          //Sends AT command with time (For creating multiple connections)
      connect_wifi("AT+CIPSERVER=1,80",100);                    //Sends AT command with time (For setting up server with port 80)
}

void sendwebdata(String webPage)                          //This function is used to send webpage datas to the localserver
{
    int ii=0;
     while(1)
     {
      unsigned int l=webPage.length();
      Serial.print("AT+CIPSEND=0,");
      Serial2.print("AT+CIPSEND=0,");
      Serial.println(l+2);
      Serial2.println(l+2);
      delay(100);
      Serial.println(webPage);                        //sends webpage data to serial monitor
      Serial2.println(webPage);                       //sends webpage data to serial2 ESP8266
      while(Serial2.available())
      {

        if(Serial2.find("OK"))
        {
          ii=11;
          break;
        }
      }
      if(ii==11)
      break;
      delay(100);
     }
}

void setup()
{
   Serial.begin(9600);                //begins serial monitor with baud rate 9600
   Serial2.begin(9600);               //begins serial communication with esp8266 with baud rate 9600 (Change according to your esp8266 module)
   wifi_init();
   Serial.println("System Ready..");
}

void loop()
{
  k=0;
  Serial.println("Please Refresh your Page");
  while(k<1000)
  {
    k++;
   while(Serial2.available())
   {
    if(Serial2.find("0,CONNECT"))
    {
      Serial.println("Start Printing");
      Send();
      Serial.println("Done Printing");
      delay(1000);
    }
  }
  delay(1);
 }
}

void Send()                                        //This function contains data to be sent to local server
{
      webpage = "<h1>Welcome to Circuit Digest</h1><body bgcolor=f0f0f0>";
      sendwebdata(webpage);
      webpage=name;
      sendwebdata(webpage);
      delay(1000);
      webpage = "<a href=\"http://circuitdigest.com/\"";
      webpage+="\">Click Here to get into circuitdigest.com</a>";
      webpage+=data;
      sendwebdata(webpage);
      Serial2.println("AT+CIPCLOSE=0");                  //Closes the server connection
}

*/




