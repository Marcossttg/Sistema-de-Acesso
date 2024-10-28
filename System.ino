#include < SPI.h >
#include < MFRC522.h >
#include < Servo.h >

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 7
#define RELAY_PIN 8
#define LED_GREEN 5
#define LED_RED 6
#define BUZZER_PIN 4

// Instância do RFID
MFRC522 rfid(SS_PIN, RST_PIN);
Servo gateServo;

// IDs dos RFID
// Substitua pelos UIDs reais
String authorizedUIDs[] = { "UID1", "UID2", "UID3"};

void setup() {
	Serial.begin(9600);
	SPI.begin();
	rfid.PCD_Init();

	// Configura LEDs
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);

	// Configura o motor
	gateServo.attach(SERVO_PIN);
	gateServo.write(0);

	Serial.println("Sistema de Acesso Automático para Motoboys Iniciado");
}

void loop() {
	// Verifica um novo cartão
	if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
		for (byte i = 0; i < rfid.uid.size; i++) {
			uid += String(rfid.uid.uidByte[i], HEX);
		}
		uid.toUpperCase();
		Serial.println("UID do Cartão: " + uid);

		// Lista de autorizados
		if (isAuthorized(uid)) {
			Serial.println("Acesso Autorizado");
			digitalWrite(LED_GREEN, HIGH);
			digitalWrite(LED_RED, LOW);
			digitalWrite(BUZZER_PIN, LOW);
			openGate();
		} else {
			Serial.println("Acesso Negado");
			digitalWrite(LED_GREEN, LOW);
			digitalWrite(LED_RED, HIGH);
			digitalWrite(BUZZER_PIN, HIGH);
			delay(1000);
			digitalWrite(BUZZER_PIN, LOW);
		}

		// Cartão removido
		while (rfid.PICC_IsNewCardPresent()) {
			delay(10);
		}
		digitalWrite(LED_GREEN, LOW);
		digitalWrite(LED_RED, LOW);
	}
}

bool isAuthorized(String uid) {
	for (String authorizedUID : authorizedUIDs) {
		if (uid == authorizedUID) {
			return true;
		}
	}
	return false;
}

void openGate() {
	gateServo.write(90);
	digitalWrite(RELAY_PIN, HIGH);
	delay(5000);
	gateServo.write(0);
	digitalWrite(RELAY_PIN, LOW);
}
