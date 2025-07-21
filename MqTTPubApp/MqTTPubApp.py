import paho.mqtt.client as mqtt
import time
import json 
import serial

# ========
# Helpers
# ========
def open_config(conf):
    with open(conf, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data

# ===========================
# Set Serial Port (/dev/tty*)
# ===========================
SERIAL_PORT = '/dev/ttyUSB0'  
BAUD_RATE = 9600            

# ================
# Set MQTT BROKER
# ================
config = open_config('./config.json')
MQTT_BROKER = config["MQTT"]["IP"]
MQTT_PORT = config["MQTT"]["PORT"]
MQTT_TOPIC = config["MQTT"]["TOPIC"] 
MQTT_CLIENT_ID = config["MQTT"]["CLIENT_ID"]

# =====================
# Define MQTT CALLBACK
# =====================
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"MQTT 브로커에 연결되었습니다. (Result Code: {rc})")
    else:
        print(f"MQTT 브로커 연결 실패 (Result Code: {rc})")

def on_publish(client, userdata, mid):
    pass 

# ===================
# Create MQTT Client
# ===================
client = mqtt.Client(client_id=MQTT_CLIENT_ID)
client.on_connect = on_connect
client.on_publish = on_publish

# =====
# main
# =====
def main():
    try:
        print(f"MQTT 브로커 {MQTT_BROKER}:{MQTT_PORT}에 연결 시도 중...")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start() # run background

        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"시리얼 포트 {SERIAL_PORT}가 성공적으로 열렸습니다.")
            
            while True:
                line = ser.readline().decode('utf-8').strip()   # 구분자('\n') Block 대기

                if line:
                    print(f"[DEBUG] readline : {line}")
                    
                    try:
                        parts = line.split(',')
                        
                        if len(parts) == 4:
                            analog_value = int(parts[0])
                            digital_value = int(parts[1])
                            humidity = float(parts[2])
                            temperature = float(parts[3])

                            # Error Code : -1.f
                            if humidity == -1.0 or temperature == -1.0:
                                sensor_data = {
                                    "analog_rain": analog_value,
                                    "digital_rain": digital_value,
                                    "humidity": None, 
                                    "temperature": None, 
                                    "dht_error": True
                                }
                                print("temp or humidity : Error Detected")
                            else:
                                sensor_data = {
                                    "analog_rain": analog_value,
                                    "digital_rain": digital_value,
                                    "humidity": humidity,
                                    "temperature": temperature,
                                    "dht_error": False
                                }
                            
                            json_payload = json.dumps(sensor_data)

                            # Publish
                            client.publish(MQTT_TOPIC, json_payload)
                            print(f"  MQTT Pub : TOPIC='{MQTT_TOPIC}', DATA='{json_payload}'")
                            
                        else:
                            print(f"  ERROR: 예상치 못한 시리얼 데이터 형식: {line}")

                    except ValueError as e:
                        print(f"  ERROR: 시리얼 데이터 변환 실패: {line} - {e}")
                    except IndexError as e:
                        print(f"  ERROR: 시리얼 데이터 파싱 중 인덱스 에러: {line} - {e}")
                # Set Delay (sec)
                time.sleep(1)

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
        # Mqtt disconnect
        if client.is_connected():
            client.loop_stop()
            client.disconnect()
            print("MQTT 연결이 종료되었습니다.")
        print("프로그램이 완전히 종료되었습니다.")



if __name__ == "__main__":
    main()