#define BLYNK_TEMPLATE_ID "TMPL6BtWNLJaz"
#define BLYNK_TEMPLATE_NAME "UAS SIstem Tertanam Cerdas"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

char ssid[] = "realme C17";
char pass[] = "88888888";

char auth[] = "1FfuFa9MNtiiHfS-6p2cKp78zqwIPs8V";

#define BOT_TOKEN "7473134371:AAHrb-q-eYIZj6K6RMrQurfx3nVXplUecMg"
#define CHAT_ID "5336616280"
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);

const int trigPin = 5;     // Trigger pin
const int echoPin = 18;    // Echo pin
const int servoPin = 19;   // Servo
const int LED_WIFI = 2;

const float containerHeight = 20.0;
const float minFoodLevel = 5.0;

// Variabel jadwal makan
int feedingTimes[3][3] = {  // {jam, menit, durasi_detik}
  {8, 0, 3},   // Pagi
  {12, 0, 3},  // Siang
  {19, 30, 3}   // Sore
};

// Variabel untuk mencegah trigger berulang
int lastTriggeredHour = -1;
int lastTriggeredMinute = -1;
bool foodEmptyNotified = false;  // Untuk mencegah notifikasi berulang saat makanan habis

Servo feederServo;
BlynkTimer timer;

// Fungsi untuk mengkonversi cm ke persen (20 cm = 100%)
float cmToPercent(float distanceCM) {
  if (distanceCM >= containerHeight) {
    return 100.0;
  } else if (distanceCM <= 0) {
    return 0.0;
  } else {
    return (distanceCM / containerHeight) * 100.0;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  feederServo.attach(servoPin);
  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, LOW);
  
  // Koneksi ke WiFi
  WiFi.begin(ssid, pass);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nTerhubung!");
  digitalWrite(LED_WIFI, HIGH);

  // Inisialisasi NTP
  timeClient.begin();
  timeClient.setTimeOffset(25200); // GMT+7
  
  // Tunggu sampai waktu tersinkronisasi
  while(!timeClient.update()) {
    timeClient.forceUpdate();
    delay(500);
  }
  
  Serial.println("Waktu NTP tersinkronisasi:");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  // Inisialisasi Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Set timer untuk pengecekan
  timer.setInterval(3000L, checkFoodLevel);      // Setiap 3 detik
  timer.setInterval(5000L, checkFeedingTime);   // Setiap 5 detik
  
  // Posisi awal servo (tertutup)
  closeFeeder();
  
  // Cek level makanan saat startup
  float startupLevel = measureFoodLevel();
  
  // Kirim pesan startup ke Telegram
  String startupMsg = "🔌 Smart Pet Feeder telah menyala!\n";
  startupMsg += "IP: " + WiFi.localIP().toString() + "\n";
  startupMsg += "Waktu sistem: " + String(timeClient.getHours()) + ":" + 
               String(timeClient.getMinutes()) + ":" + 
               String(timeClient.getSeconds()) + "\n";
  startupMsg += "Status Makanan: " + String(startupLevel, 1) + " cm (" + 
               String(cmToPercent(startupLevel), 1) + "%)\n";
  startupMsg += "Jadwal Makan:\n";
  startupMsg += "1. 08:00 (3 detik)\n";
  startupMsg += "2. 12:00 (3 detik)\n";
  startupMsg += "3. 19:30 (3 detik)";
  
  bot.sendMessage(CHAT_ID, startupMsg, "");
  
  // Jika makanan habis saat startup
  if (startupLevel <= 0.00) {
    bot.sendMessage(CHAT_ID, "🚨 PERINGATAN: Makanan HABIS terdeteksi saat startup!", "");
    foodEmptyNotified = true;
  }
}

void loop() {
  Blynk.run();
  timer.run();
  
  // Handle incoming Telegram messages
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while(numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

// Fungsi untuk mengukur level makanan
float measureFoodLevel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // Konversi ke cm
  
  // Jika jarak melebihi tinggi container, anggap sebagai 0 cm (makanan habis)
  if (distance >= containerHeight) {
    distance = containerHeight;
  }
  
  // Hitung tinggi makanan tersisa
  float foodLevel = containerHeight - distance;
  if (foodLevel < 0) foodLevel = 0;
  
  // Hitung persentase
  float foodPercent = cmToPercent(foodLevel);
  
  Serial.print("Jarak terukur: ");
  Serial.print(distance);
  Serial.print(" cm, Makanan tersisa: ");
  Serial.print(foodLevel);
  Serial.print(" cm (");
  Serial.print(foodPercent);
  Serial.println("%)");
  
  // Update ke Blynk
  Blynk.virtualWrite(V0, foodLevel);       // Nilai dalam cm
  Blynk.virtualWrite(V1, foodPercent);     // Nilai dalam persen
  
  return foodLevel;
}

// Fungsi untuk mengecek level makanan
void checkFoodLevel() {
  float currentLevel = measureFoodLevel();
  float currentPercent = cmToPercent(currentLevel);
  
  // Jika makanan habis (0.00 cm)
  if (currentLevel <= 0.00) {
    if (!foodEmptyNotified) {
      String warning = "🚨 PERINGATAN: Makanan HABIS!\n";
      warning += "Sisa: " + String(currentLevel, 1) + " cm (" + String(currentPercent, 1) + "%)";
      
      bot.sendMessage(CHAT_ID, warning, "");
      foodEmptyNotified = true;  // Set flag agar tidak mengirim notifikasi berulang
    }
  } 
  // Jika makanan hampir habis
  else if (currentLevel < minFoodLevel) {
    String warning = "⚠️ PERINGATAN: Makanan hampir habis!\n";
    warning += "Sisa: " + String(currentLevel, 1) + " cm (" + String(currentPercent, 1) + "%)";
    
    bot.sendMessage(CHAT_ID, warning, "");
    foodEmptyNotified = false;  // Reset flag jika makanan diisi ulang
  }
  // Jika makanan di atas level minimum
  else {
    foodEmptyNotified = false;  // Reset flag jika makanan diisi ulang
  }
}

// Fungsi untuk mengecek jadwal makan
void checkFeedingTime() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  for (int i = 0; i < 3; i++) {
    if (currentHour == feedingTimes[i][0] && 
        currentMinute == feedingTimes[i][1] &&
        !(currentHour == lastTriggeredHour && currentMinute == lastTriggeredMinute)) {
      
      lastTriggeredHour = currentHour;
      lastTriggeredMinute = currentMinute;
      
      feedPet(feedingTimes[i][2]);
      delay(5000); // Delay untuk hindari trigger berulang
      return;
    }
  }
}

// Fungsi untuk memberi makan
void feedPet(int durationSeconds) {
  openFeeder();
  
  String message = "🍽️ Memberi makan pada ";
  message += String(timeClient.getHours()) + ":";
  message += String(timeClient.getMinutes()) + " selama ";
  message += String(durationSeconds) + " detik";
  bot.sendMessage(CHAT_ID, message, "");
  
  delay(durationSeconds * 1000);
  closeFeeder();
  checkFoodLevel();
}

void openFeeder() {
  feederServo.write(90);
  Blynk.virtualWrite(V5, 1);
}

void closeFeeder() {
  feederServo.write(0);
  Blynk.virtualWrite(V5, 0);
}

// Fungsi untuk menangani pesan Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    if (text == "/status") {
      float foodLevel = measureFoodLevel();
      float foodPercent = cmToPercent(foodLevel);
      String status = "📊 Status Feeder:\n";
      status += "• Makanan: " + String(foodLevel, 1) + " cm (" + String(foodPercent, 1) + "%)\n";
      status += "• Waktu sistem: " + String(timeClient.getHours()) + ":" + 
               String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()) + "\n";
      status += "• Jadwal makan:\n";
      for (int j = 0; j < 3; j++) {
        status += "  " + String(j+1) + ". " + String(feedingTimes[j][0]) + ":" + 
                 (feedingTimes[j][1] < 10 ? "0" : "") + String(feedingTimes[j][1]) + 
                 " (" + String(feedingTimes[j][2]) + " detik)\n";
      }
      bot.sendMessage(CHAT_ID, status, "");
    }
    else if (text == "/feed") {
      feedPet(3);
      bot.sendMessage(CHAT_ID, "Memberi makan manual dilakukan", "");
    }
    else if (text == "/time") {
      timeClient.update();
      String timeMsg = "🕒 Waktu sistem: ";
      timeMsg += String(timeClient.getHours()) + ":";
      timeMsg += String(timeClient.getMinutes()) + ":";
      timeMsg += String(timeClient.getSeconds());
      bot.sendMessage(CHAT_ID, timeMsg, "");
    }
    else if (text == "/schedule") {
      String scheduleMsg = "⏰ Jadwal Makan Saat Ini:\n";
      for (int j = 0; j < 3; j++) {
        scheduleMsg += String(j+1) + ". " + String(feedingTimes[j][0]) + ":" + 
                     (feedingTimes[j][1] < 10 ? "0" : "") + String(feedingTimes[j][1]) + 
                     " (" + String(feedingTimes[j][2]) + " detik)\n";
      }
      bot.sendMessage(CHAT_ID, scheduleMsg, "");
    }
  }
}

// Fungsi Blynk untuk mengatur jadwal makan
BLYNK_WRITE(V10) { // Jam makan 1
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    feedingTimes[0][0] = t.getStartHour();
    feedingTimes[0][1] = t.getStartMinute();
    String msg = "🔄 Jadwal makan Pagi diupdate ke ";
    msg += String(feedingTimes[0][0]) + ":";
    msg += feedingTimes[0][1] < 10 ? "0" : "";
    msg += String(feedingTimes[0][1]);
    bot.sendMessage(CHAT_ID, msg, "");
  }
}

BLYNK_WRITE(V11) { // Durasi makan 1
  feedingTimes[0][2] = param.asInt();
  bot.sendMessage(CHAT_ID, "⏱ Durasi makan Pagi diupdate: " + String(feedingTimes[0][2]) + " detik", "");
}

BLYNK_WRITE(V20) { // Jam makan 2
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    feedingTimes[1][0] = t.getStartHour();
    feedingTimes[1][1] = t.getStartMinute();
    String msg = "🔄 Jadwal makan Siang diupdate ke ";
    msg += String(feedingTimes[1][0]) + ":";
    msg += feedingTimes[1][1] < 10 ? "0" : "";
    msg += String(feedingTimes[1][1]);
    bot.sendMessage(CHAT_ID, msg, "");
  }
}

BLYNK_WRITE(V21) { // Durasi makan 2
  feedingTimes[1][2] = param.asInt();
  bot.sendMessage(CHAT_ID, "⏱ Durasi makan Siang diupdate: " + String(feedingTimes[1][2]) + " detik", "");
}

BLYNK_WRITE(V30) { // Jam makan 3
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    feedingTimes[2][0] = t.getStartHour();
    feedingTimes[2][1] = t.getStartMinute();
    String msg = "🔄 Jadwal makan Sore diupdate ke ";
    msg += String(feedingTimes[2][0]) + ":";
    msg += feedingTimes[2][1] < 10 ? "0" : "";
    msg += String(feedingTimes[2][1]);
    bot.sendMessage(CHAT_ID, msg, "");
  }
}

BLYNK_WRITE(V31) { // Durasi makan 3
  feedingTimes[2][2] = param.asInt();
  bot.sendMessage(CHAT_ID, "⏱ Durasi makan Sore diupdate: " + String(feedingTimes[2][2]) + " detik", "");
}

BLYNK_WRITE(V40) { // Tombol feed manual
  if (param.asInt()) {
    feedPet(3);
    Blynk.virtualWrite(V40, 0);
  }
}