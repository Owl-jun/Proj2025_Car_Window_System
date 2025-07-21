import paho.mqtt.client as mqtt
import time
import json # JSON 형식으로 데이터 전송을 위해 임포트

# --- ReceiveData.py에서 시리얼 데이터 수신 함수를 임포트 ---
# 주의: 실제 프로젝트에서는 ReceiveData.py의 핵심 로직을 함수로 만들고,
#       그 함수를 여기서 호출하는 것이 좋습니다.
#       여기서는 예시를 위해 ReceiveData.py의 Serial.readline() 부분을 직접 가져와 사용합니다.
#       만약 ReceiveData.py가 class나 다른 함수 형태로 되어 있다면, 그에 맞춰 import 및 호출 방식을 변경해야 합니다.

# 이전 ReceiveData.py 코드의 시리얼 통신 부분을 직접 가져옴
# (실제 파일 구조에 따라 './read_arduino.py'와 같이 경로를 지정해야 할 수도 있습니다.)
import serial

# --- 시리얼 포트 설정 (ReceiveData.py와 동일하게 설정) ---
SERIAL_PORT = '/dev/ttyS0'  # 또는 '/dev/ttyUSB0' 등 올바른 포트로 변경
BAUD_RATE = 9600            # 아두이노와 동일하게 9600 보드레이트 설정


# --- MQTT 브로커 설정 ---
MQTT_BROKER = "210.119.12.82"  # 라즈베리 파이에 Mosquitto가 설치되어 있다면 localhost
MQTT_PORT = 1883           # 기본 MQTT 포트
MQTT_TOPIC = "sensor/rain_dht" # 데이터를 발행할 토픽 (원하는 이름으로 변경 가능)
MQTT_CLIENT_ID = "RaspberryPi_SensorPublisher" # 클라이언트 ID (고유해야 함)


# --- MQTT 콜백 함수 정의 ---
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"MQTT 브로커에 연결되었습니다. (Result Code: {rc})")
    else:
        print(f"MQTT 브로커 연결 실패 (Result Code: {rc})")

def on_publish(client, userdata, mid):
    # print(f"메시지 발행 완료 (MID: {mid})")
    pass # 발행 성공 메시지는 너무 많이 출력될 수 있으므로 주석 처리하거나 빈 함수로 둠

# --- MQTT 클라이언트 초기화 ---
client = mqtt.Client(client_id=MQTT_CLIENT_ID)
client.on_connect = on_connect
client.on_publish = on_publish

# --- 메인 실행 로직 ---
def main():
    try:
        # MQTT 브로커 연결
        print(f"MQTT 브로커 {MQTT_BROKER}:{MQTT_PORT}에 연결 시도 중...")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start() # 백그라운드에서 MQTT 통신을 처리하기 위해 루프 시작

        # 시리얼 포트 열기
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"시리얼 포트 {SERIAL_PORT}가 열렸습니다. 아두이노 데이터 수신 대기 중...")
            
            while True:
                # 1. 시리얼 데이터 수신
                line = ser.readline().decode('utf-8').strip()

                if line:
                    print(f"시리얼 수신: {line}") # 디버깅용 출력
                    
                    try:
                        # 2. 데이터 파싱
                        parts = line.split(',')
                        
                        if len(parts) == 4:
                            analog_value = int(parts[0])
                            digital_value = int(parts[1])
                            humidity = float(parts[2])
                            temperature = float(parts[3])

                            # 3. 데이터를 JSON 형식으로 변환
                            # 아두이노에서 -1.0으로 보낸 오류 값은 여기서 None이나 다른 방식으로 처리 가능
                            if humidity == -1.0 or temperature == -1.0:
                                sensor_data = {
                                    "analog_rain": analog_value,
                                    "digital_rain": digital_value,
                                    "humidity": None, # 오류 시 None으로 표시
                                    "temperature": None, # 오류 시 None으로 표시
                                    "dht_error": True
                                }
                                print("  온습도 센서 오류 감지. 데이터를 None으로 설정.")
                            else:
                                sensor_data = {
                                    "analog_rain": analog_value,
                                    "digital_rain": digital_value,
                                    "humidity": humidity,
                                    "temperature": temperature,
                                    "dht_error": False
                                }
                            
                            json_payload = json.dumps(sensor_data)

                            # 4. MQTT로 발행
                            client.publish(MQTT_TOPIC, json_payload)
                            print(f"  MQTT 발행 성공: 토픽='{MQTT_TOPIC}', 데이터='{json_payload}'")
                            
                        else:
                            print(f"  오류: 예상치 못한 시리얼 데이터 형식: {line}")

                    except ValueError as e:
                        print(f"  오류: 시리얼 데이터 변환 실패: {line} - {e}")
                    except IndexError as e:
                        print(f"  오류: 시리얼 데이터 파싱 중 인덱스 에러: {line} - {e}")

                time.sleep(0.01) # 짧은 딜레이

    except serial.SerialException as e:
        print(f"시리얼 포트 오류: {e}")
        print("시리얼 포트 경로, 연결 상태, 권한을 확인하세요.")
    except ConnectionRefusedError:
        print("MQTT 브로커에 연결할 수 없습니다. 브로커가 실행 중인지 확인하세요.")
    except KeyboardInterrupt:
        print("\n프로그램을 종료합니다.")
    except Exception as e:
        print(f"예기치 않은 오류 발생: {e}")
    finally:
        # 프로그램 종료 시 MQTT 연결 종료
        if client.is_connected():
            client.loop_stop()
            client.disconnect()
            print("MQTT 연결이 종료되었습니다.")
        print("프로그램이 완전히 종료되었습니다.")

if __name__ == "__main__":
    main()