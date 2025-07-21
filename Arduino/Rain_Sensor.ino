// 핀 정의
const int ANALOG_PIN = A0; // 센서의 AS 핀을 아두이노의 아날로그 핀 A0에 연결
const int DIGITAL_PIN = 2; // 센서의 TTL 핀을 아두이노의 디지털 핀 2에 연결

// 온습도 센서 관련 핀 및 라이브러리 정의
#include "DHT.h" // DHT 라이브러리 포함

#define DHTPIN 7       // 온습도 센서의 데이터 핀을 아두이노 디지털 핀 7에 연결
#define DHTTYPE DHT11  // 사용하는 센서 타입 (DHT11 또는 DHT22 등)

DHT dht(DHTPIN, DHTTYPE); // DHT 객체 생성

void setup() {
  // 시리얼 통신 초기화 (라즈베리 파이와 동일한 보드레이트 사용)
  Serial.begin(9600); 

  // TTL 핀을 입력으로 설정
  pinMode(DIGITAL_PIN, INPUT);
  // TH_PIN (온습도 센서 핀)은 DHT 라이브러리에서 자체적으로 설정하므로 별도 pinMode는 필요 없음

  // DHT 센서 시작
  dht.begin();
}

void loop() {
  // 1. 비 감지 센서 값 읽기
  int analogValue = analogRead(ANALOG_PIN);
  int digitalValue = digitalRead(DIGITAL_PIN);

  // 2. 온습도 센서 값 읽기
  // 온습도 센서는 너무 자주 읽으면 정확도가 떨어질 수 있으므로, 약간의 지연을 두는 것이 좋습니다.
  // 여기서는 루프의 delay(100) 외에 별도 delay는 넣지 않았습니다.
  // 만약 DHT 센서 읽기 오류가 발생하면, delay(2000); 와 같이 더 긴 지연을 고려해 보세요.
  float humidity = dht.readHumidity();    // 습도 읽기
  float temperature = dht.readTemperature(); // 온도 읽기 (섭씨)

  // 값 읽기 실패 시 NaN 반환 (Not a Number)
  if (isnan(humidity) || isnan(temperature)) {
    // Serial.println("온습도 센서에서 데이터를 읽는 데 실패했습니다.");
    // 실패 시에는 특정 값을 보내거나 (예: -999), 이전 값을 유지하거나, 데이터를 보내지 않을 수 있습니다.
    // 여기서는 실패하더라도 현재까지 읽은 비 감지 센서 데이터라도 보내도록 합니다.
    humidity = -1.0; // 오류 발생 시 특정 값으로 설정하여 라즈베리 파이에서 구분하도록 함
    temperature = -1.0;
  }

  // 3. 시리얼 포트를 통해 라즈베리 파이로 데이터 전송
  // 형식: 아날로그값,디지털값,습도,온도\n
  Serial.print(analogValue);
  Serial.print(",");
  Serial.print(digitalValue);
  Serial.print(",");
  Serial.print(humidity); // 습도 값 전송
  Serial.print(",");
  Serial.println(temperature); // 온도 값 전송 후 줄바꿈

  // 4. 데이터 전송 간격 설정
  delay(100); 
}