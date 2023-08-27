#include <Arduino.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <CTBot.h>

#include "gettimesetting.h"
#include "wificonnect.h"
#include "timefunction.h"

/* WIFI SSID & Password */
const char* ssid;  // Enter SSID here
const char* password;  //Enter Password here
const char* deviceName;

/*  GPIO SETUP */
// Init pin number on ESP8266 board
// #define connled 2
// #define saklar 13
// #define secIndicator 12

// Init pin number on ESP01 board
#define buzz 1 //TX
#define saklar 3 // RX / Saklar
#define connled 0
#define secIndicator 2

/*  NTP SETUP */
// Iniotialize NTP Class
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

  uint8_t waktu1Awal;
  uint8_t waktu1Akhir;

// Clock variable
unsigned long timeLast;
unsigned long previousMillis;
unsigned long updatetimeTemp; // Temporary time for updating saved time
const unsigned long intervalDays = 24*60*60; // 1 Days interval 60sec*60min*24hour
uint8_t const intervalSec = 5;

// set your starting hour here, not below at int hour. This ensures accurate daily correction of time
uint8_t seconds;
uint8_t minutes;
uint8_t hours;
uint16_t days;

/*  TELEGRAM BOT SETUP */
CTBot myBot;
TBMessage msg;
// String token = "5818046411:AAEv_uG0gr_Sn3eUncC5cMo3IkvBVsR73Pk";   // (my_kubot TOKEN) REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
String token = "6438525941:AAGqB556_PuiyxSOMCnWD3HNunM9EBj0fMY";   // (puzzletesbot TOKEN)
CTBotInlineKeyboard myKbd;   // reply keyboard object helper

/* Init CTBot query */
#define LIGHT_ON_CALLBACK  "ON1"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "OFF1" // callback data sent when "LIGHT OFF" button is pressed 
#define RESET_CALLBACK "RESET"

// Make function prototype
void updateTime(unsigned long currentMillis);
void showTime(unsigned long currentMillis);
void timerFunction();
void telegramOperation(unsigned long currentTime);
void telegramKeyboard();

void setup() {
 /*  Common cathode led RGB */
  Serial.begin(115200);

  //  Initialize IO pin -----------------------
  pinMode(connled, OUTPUT);
  pinMode(saklar, OUTPUT);
  pinMode(secIndicator, OUTPUT);
  pinMode(buzz, OUTPUT);

  //  Connecting to wifi -----------------------
  /* WIFI KANTOR 
  ssid = "Puzzle24";  // Enter SSID here
  password = "gzcmb94463";  // Enter Password here
  deviceName = "Smart Switch"; // DHCP Hostname (useful for finding device for static lease) 
  */

  /* WIFI KOS   */
  // ssid = "PinkTulip";  // Enter SSID here
  // password = "Bekasikota97";  // Enter Password here
  ssid = "Puzzle24";  // Enter SSID here
  password = "gzcmb94463";  // Enter Password here
  deviceName = "Smart Switch"; // DHCP Hostname (useful for finding device for static lease) 


  wificonnect(ssid, password, deviceName, connled);

  Serial.println(WiFi.gatewayIP());

  	// Initialize filesystem -----------------------
	if(!LittleFS.begin()){
		Serial.println("An Error has occurred while mounting LittleFS");
		return;
	}

	// Testing filesystem -----------------------
	File file = LittleFS.open("time.csv", "r");
	if(!file){
		Serial.println("Failed to open file for reading");
		return;
	}
	Serial.println("File Content:");
	while(file.available()){
		Serial.write(file.read());
	}
	file.close();

  // Get time setting from filesystem -----------------------
	getTimesetting(waktu1Awal, waktu1Akhir);

	//  Connecting to telegram -----------------------
	myBot.setTelegramToken(token);
	// check if all things are ok
	if (myBot.testConnection())
		Serial.println("\nTest connection Succes");
	else
		Serial.println("\nTest connection Failed");

  //  NTP Server begin -----------------------
  timeClient.begin();
  // Tambahan fungsi
  timeClient.setTimeOffset(25200);
  timeClient.setUpdateInterval(180000);

  // Dapatkan waktu pertama kali dari NTP Server -----------------------
  timeClient.update();
  seconds = timeClient.getSeconds();
  minutes = timeClient.getMinutes();
  hours = timeClient.getHours();

  // Calling Keyboard button class -----------------------
	telegramKeyboard();

  // String timerstartStr = String(jamTimer[0][0]);
  // String timerstopStr = String(jamTimer[0][1]);
  // Serial.println(timerstartStr);
  // Serial.println(timerstopStr);
}

void loop() {
  // Millis Function #1
  unsigned long currentMillis = millis() / 1000;  // the number of milliseconds that have passed since boot
  
  telegramOperation(currentMillis);
  clockCounter(currentMillis, previousMillis, seconds, minutes, hours, days);
  updateTime(currentMillis);
  showTime(currentMillis, timeLast, intervalSec, secIndicator);
}

void updateTime(unsigned long currentMillis) {
  // update waktu ke server NTP
  if (currentMillis - updatetimeTemp >= intervalDays) {
    updatetimeTemp = currentMillis;

    timeClient.begin();
    // Dapatkan waktu dan update ke variabel dari NTP Server -----------------------
    timeClient.update();
    seconds = timeClient.getSeconds();
    minutes = timeClient.getMinutes();
    hours = timeClient.getHours();
  }
}

void timerFunction() {
  // Fungsi timer utama
  for (int i=0; i<2; i++) {
    if(hours == waktu1Awal) {
        // Saklar menggunakan NORMALY CLOSED jadi logikanya dibalik, kalo high mati
        digitalWrite(saklar, LOW);
      } else if(hours == waktu1Akhir) {
        digitalWrite(saklar, HIGH);
    }
  }
}

void telegramOperation(unsigned long currentMillis) {
	TBMessage msg;

	// 3 Relay switch
	String welcome = "Halo\n";
	welcome += "Ini adalah bot untuk app esp8266\n";
	welcome += "ketik /Keyboard untuk menampilkan menu\n";
	welcome += "ketik /Time untuk waktu timer berjalan\n";
	welcome += "ketik /CheckTime untuk melihat catatan waktu tersimpan\n";
	welcome += "Format pengaturan waktu alarm: WAKTUAWAL1#WAKTUAKHIR1-WAKTUAWAL2#WAKTUAKHIR2\n\n";

  String hoursStr = String(hours);
  String minutesStr = String(minutes);
  String secondsStr = String(seconds);

	if(myBot.getNewMessage(msg)) {
		if(msg.messageType == CTBotMessageText) {
			if(msg.text.equalsIgnoreCase("/Start") || msg.text.equalsIgnoreCase("/Keyboard")) {
				myBot.sendMessage(msg.sender.id, welcome, myKbd);
           // Indikator suara terima data
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
			} else if (msg.text.equalsIgnoreCase("/Time")) {
        // Indikator suara terima data
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
				myBot.sendMessage(msg.sender.id, "Waktu internal: " + hoursStr + ":" + minutesStr + ":" + secondsStr);
			} else if (msg.text.equalsIgnoreCase("/CheckTime")) {
        // Indikator suara terima data
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
        getTimesetting(waktu1Awal, waktu1Akhir);
				myBot.sendMessage(msg.sender.id, "Waktu Timer #1: " + String(waktu1Awal) + " sampai " + String(waktu1Akhir));
      } else {
				String inputWaktu = msg.text;

        uint8_t pembatas1 = inputWaktu.indexOf("#");

        uint8_t waktu1Awal = inputWaktu.substring(0, pembatas1).toInt();
        uint8_t waktu1Akhir = inputWaktu.substring(pembatas1+1, inputWaktu.length()).toInt();

        if ((waktu1Awal < waktu1Akhir)) {
          File file = LittleFS.open("time.csv", "w");
					//Write to the file
					file.print(inputWaktu);
					delay(1);
					//Close the file
					file.close();

					getTimesetting(waktu1Awal, waktu1Akhir);
					myBot.sendMessage(msg.sender.id, "Waktu Timer #1: " + String(waktu1Awal) + " sampai " + String(waktu1Akhir));
					myBot.sendMessage(msg.sender.id, "Pengaturan tersimpan, harap mereset perangkat.....");
          // Indikator suara terima data
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
        } else {
          myBot.sendMessage(msg.sender.id, "Error, keyword tidak ditemukan atau format waktu tidak valid (enit ke 2 lebih kecil daripada menit ke 1)");
          // Indikator suara terima data
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
          digitalWrite(buzz, HIGH);
          delay(150);
          digitalWrite(buzz, LOW);
          delay(150);
        }
			}
		} else if (msg.messageType == CTBotMessageQuery) {
			if (msg.callbackQueryData.equals(LIGHT_ON_CALLBACK)) {
				// pushed "LIGHT ON" button...
				digitalWrite(saklar, LOW);
				// terminate the callback with an alert message
        digitalWrite(saklar, HIGH);
        // Untuk keluar sementara bisa dicoba panggil fungsi baru dan di loop di situ sampai ada instruksi masuk lagi dari telegram untuk keluar dari loop yang sudah ada
				myBot.endQuery(msg.callbackQueryID, "Light on", true);
			} else if (msg.callbackQueryData.equals(LIGHT_OFF_CALLBACK)) {
				// pushed "LIGHT OFF" button...
				digitalWrite(saklar, HIGH);
				// terminate the callback with a popup message
				myBot.endQuery(msg.callbackQueryID, "Light off", true);
			}
		}
	}
}

void telegramKeyboard() {
	//  Creating Keyboard button class -----------------------
	myKbd.addButton("Lampu Nyala", LIGHT_ON_CALLBACK, CTBotKeyboardButtonQuery);
	myKbd.addButton("Lampu Mati", LIGHT_OFF_CALLBACK, CTBotKeyboardButtonQuery);
	// myKbd.addRow();
}