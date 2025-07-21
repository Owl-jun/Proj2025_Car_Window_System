# MqTT Publish App
Python을 활용한 Serial 수신 및 mqtt-paho 라이브러리를 통해 Publish 하는 앱 입니다.

## 개요

- 라이브러리 버전 충돌 등을 방지하기 위해 venv 환경을 구축하고 사용하였습니다.
```
cd directory
python -m venv venv
source venv/bin/activate
```

- config.json 파일을 통해 MQTT 세팅을 쉽게 변경할 수 있게끔 설계하였습니다.
    - 추후 gitignore에 json 을 숨김으로서, 보안 적용이 가능합니다.

    - config.json

        ![](https://velog.velcdn.com/images/owljun/post/afaf796a-ceac-4ebe-a21a-d0da451a9ef8/image.png)
    
    - json 파싱 Helper 함수 및 파싱 코드
        
        ![](https://velog.velcdn.com/images/owljun/post/795d2933-c3b5-46f1-a059-14bba0ccafee/image.png)


- Delimiter (문자열 구분자 '\n') 을 기반으로 데이터를 수신합니다.
    - 추후 기능 확장시, 복잡한 시스템이 도입되면 TLP와 같은 패킷 디자인을 적용하여 리팩토링할 계획입니다.
    - readline 을 활용한 데이터 수신
    
    ![](https://velog.velcdn.com/images/owljun/post/2e407880-1ea9-4175-b58b-5f16ba7da976/image.png)

- 에러 핸들링
    - 에러 발생시 json 데이터를 다음과 같게끔 설정하여 Publish 합니다. 이를 통해 Sub하는 App들은 에러 여부를 판단할 수 있습니다.

    ![](https://velog.velcdn.com/images/owljun/post/4b94f29b-d747-4463-914b-b44e71717897/image.png)

    - 에러를 종류별로 나누어 디버깅이 수월하게끔 관리해주었습니다.

    ![](https://velog.velcdn.com/images/owljun/post/dc14b300-6cf7-49b1-ac2e-3e88c563c705/image.png)


## 실행

- 아두이노 -> 파이4 데이터 수신과 Pub 까지 하는 과정을 테스트 했습니다.

    - MQTT Explorer 데이터 수신 확인

    ![](https://velog.velcdn.com/images/owljun/post/845a8a91-92b4-4d67-9ca0-1ed800db129f/image.png)

    - SSH 터미널 DEBUG 로그 확인

    ![](https://velog.velcdn.com/images/owljun/post/c6ee620c-04ab-434b-920f-a585f8ef949b/image.png)