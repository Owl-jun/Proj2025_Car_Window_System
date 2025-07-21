# Arduino Uno Rain_Sensor Src
DHT11 , 비 감지 센서 센싱 데이터 -> Serial 통신으로 Raspi 4 전송하는 소스코드 입니다.

## 특징

- 가독성 및 유지보수를 위한 define 및 주석 코드스타일

![](https://velog.velcdn.com/images/owljun/post/b832f886-5aae-4404-8405-4327c2c1ce5a/image.png)

- 시스템 내부 모든 프로그램에서 에러코드를 -1로 정의하고 파싱합니다.
- 에러 핸들링은 라즈베리파이4 에서 하게끔 설계하였습니다.

![](https://velog.velcdn.com/images/owljun/post/5ba8b100-09bf-437b-8801-ed909a1507ff/image.png)