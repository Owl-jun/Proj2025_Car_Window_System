import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time
import json 

# ========
# SET GPIO
# ========
M_SIG = 26
ISRAIN = False
RUN = False
DIR = 0
TARGET_ANGLE = 0
GPIO.setmode(GPIO.BCM)
GPIO.setup(M_SIG,GPIO.OUT)
GPIO.setwarnings(False)
pwm = GPIO.PWM(M_SIG, 50) 
pwm.start(0)

# ========
# Helpers
# ========
def open_config(conf):
    with open(conf, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data

# ================
# Set MQTT BROKER
# ================
config = open_config('./secure/config.json')
MQTT_BROKER = config["MQTT"]["IP"]
MQTT_PORT = config["MQTT"]["PORT"]
MQTT_TOPIC = config["MOTOR"]["TOPIC"] 
MQTT_CLIENT_ID = config["MOTOR"]["CLIENT_ID"]

# =====================
# Define MQTT CALLBACK
# =====================
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"MQTT 브로커에 연결되었습니다. (Result Code: {rc})")
    else:
        print(f"MQTT 브로커 연결 실패 (Result Code: {rc})")

def on_message(client, userdata, msg):
    global RUN, DIR, ISRAIN
    try:
        payload = msg.payload.decode('utf-8').strip()
        val = int(payload)

        if val == 2:
            print("[MQTT] 비 감지 코드 수신")
            ISRAIN = True

        elif val == 3:
            print("[MQTT] 비 그침 코드 수신")
            ISRAIN = False

        elif val == 1:
            print("[MQTT] UP 명령 수신")
            RUN = True
            DIR = 1

        elif val == -1:
            print("[MQTT] DOWN 명령 수신")
            RUN = True
            DIR = -1

        elif val == 0:
            print("[MQTT] STOP 명령 수신")
            RUN = False
            DIR = 0

    except Exception as e:
        print(f"[ERROR] MQTT 메시지 파싱 실패: {e}")

# ===================
# Create MQTT Client
# ===================
client = mqtt.Client(client_id=MQTT_CLIENT_ID)
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe(MQTT_TOPIC)
client.loop_start()

# =========================
# 각도 -> 듀티비 변환 함수
# =========================
def set_angle(angle):
    global TARGET_ANGLE
    TARGET_ANGLE = angle
    angle = max(0, min(180, angle))
    duty = 2 + (angle / 18)
    pwm.ChangeDutyCycle(duty)
    time.sleep(0.3)
    pwm.ChangeDutyCycle(0)


if __name__ == "__main__":
    try:
        print("MQTT 제어 대기 중...")
        while True:
            if RUN:
                if DIR == 1:
                    if (TARGET_ANGLE == 180) : pass
                    else : 
                        set_angle(180)  # UP
                elif DIR == -1:
                    if (TARGET_ANGLE == 0) : pass
                    else : set_angle(0)  # DOWN
            time.sleep(0.1)  # 시스템 보호하기 위한 딜레이

    except KeyboardInterrupt:
        print("종료")

    finally:
        pwm.stop()
        GPIO.cleanup()
        if client.is_connected():
            client.loop_stop()
            client.disconnect()
            print("MQTT 연결 종료")
        print("프로그램 종료")