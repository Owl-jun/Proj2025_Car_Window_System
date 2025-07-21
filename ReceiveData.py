import serial
import time

# --- 시리얼 포트 설정 ---
# 라즈베리 파이의 시리얼 포트 경로를 확인하여 설정합니다.
# 일반적으로 Raspberry Pi 3, 4의 경우 /dev/ttyS0 또는 /dev/ttyUSB0 (USB-시리얼 변환기 사용 시) 입니다.
# 'ls /dev/tty*' 명령어로 확인해 보세요.
SERIAL_PORT = '/dev/ttyUSB0'  # 또는 '/dev/ttyUSB0' 등 올바른 포트로 변경
BAUD_RATE = 9600            # 아두이노와 동일하게 9600 보드레이트 설정

def read_sensor_data():
    """
    시리얼 포트에서 데이터를 읽고 파싱하는 함수
    """
    try:
        # 시리얼 포트 열기
        # with 문을 사용하면 블록을 벗어날 때 포트가 자동으로 닫힙니다.
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"시리얼 포트 {SERIAL_PORT}가 열렸습니다. 아두이노 데이터 수신 대기 중...")
            
            while True:
                # 라인(줄바꿈 문자 '\n'까지)을 읽습니다.
                # 아두이노에서 println()을 사용했으므로 줄바꿈으로 데이터가 구분됩니다.
                line = ser.readline().decode('utf-8').strip()

                if line:
                    print(f"수신된 원본 데이터: {line}")
                    
                    try:
                        # 콤마(,)로 분리하여 각 값을 가져옵니다.
                        # 형식: 아날로그값,디지털값,습도,온도
                        parts = line.split(',')
                        
                        if len(parts) == 4:
                            analog_value = int(parts[0])
                            digital_value = int(parts[1])
                            humidity = float(parts[2])
                            temperature = float(parts[3])

                            print(f"  아날로그 값 (비 감지): {analog_value}")
                            print(f"  디지털 값 (비 감지): {digital_value}")
                            
                            # 온습도 센서 오류 값 처리 (-1.0은 아두이노에서 오류 시 보낸 값)
                            if humidity == -1.0 or temperature == -1.0:
                                print("  온습도 센서 데이터 읽기 실패 (아두이노 측 오류)")
                            else:
                                print(f"  습도: {humidity:.1f}%")
                                print(f"  온도: {temperature:.1f}°C")
                            
                            print("-" * 30) # 구분선 출력
                            
                        else:
                            print(f"  오류: 예상치 못한 데이터 형식 ({len(parts)}개의 파트): {line}")

                    except ValueError as e:
                        print(f"  오류: 수신된 데이터를 숫자로 변환할 수 없습니다: {line} - {e}")
                    except IndexError as e:
                        print(f"  오류: 데이터 파싱 중 인덱스 에러 발생: {line} - {e}")

                # 짧은 딜레이를 주어 CPU 사용량을 낮춥니다.
                time.sleep(1)

    except serial.SerialException as e:
        print(f"시리얼 포트 오류: {e}")
        print("포트 경로가 올바른지, 아두이노가 연결되었는지, 권한이 있는지 확인하세요.")
        print("권한 문제 시 'sudo chmod 666 /dev/ttyS0' (또는 해당 포트)를 시도해 볼 수 있습니다.")
    except KeyboardInterrupt:
        print("\n프로그램을 종료합니다.")
    except Exception as e:
        print(f"예기치 않은 오류 발생: {e}")

if __name__ == "__main__":
    read_sensor_data()