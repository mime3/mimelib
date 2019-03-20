----------------------------------------------------------------
Lib
----------------------------------------------------------------
LanServer
    
    내부 네트워크간 통신을 위한 서버 클래스
    
LanClient
    
    내부 네트워크간 통신을 위한 클라이언트 클래스 

MonitorLanClient
    
    내부 네트워크통신으로 성능모니터 정보를 1초마다 전송해주는 클래스
    
NetServer
    
    LanServer에서 패킷암호화 기능이 추가되고 더 많은 세션을 받을 수 있는 서버 클래스
    
NetClient
    
    LanClient에서 패킷암호화 기능이 추가된 클라이언트 클래스
    
struct.h
   
   위의 클래스에서 사용될 세션 구조체가 정의된 파일
    

Parser
    
    파일의 내용을 파싱해주는 클래스
    UNICODE 미지원
    
Clock
    
    시계 클래스
    현재시간을 문자열로 리턴해주는 기능만 포함중
    
CpuUsage
    
    CPU사용량 계산해주는 클래스 
    
CrashDump
    
    크래시 발생시 메모리덤프파일을 생성하도록 하는 클래스
    
DBConnector
    
    mysql, mariaDB와 연결, 쿼리전송을 해주는 클래스
    멀티스레드에 안전한 DBConnectorTLS 포함
    
HTTP_Request
    
    C++환경에서 HTTP요청, 응답을 처리해주는 클래스
    현재 POST방식만 가능
    
Logger
    
    로그파일을 작성해주는 클래스
    
LockFreeQueue
    
    락프리 구조의 큐
    
LockFreeStack
    
    락프리 구조의 스택
    
MemoryPool
    
    프리리스트 방식의 메모리 풀
    내부적으로 락프리스택을 구현하여 사용
    멀티스레드에 안전한 MemoryPoolTLS 포함
    
PDHClass
    
    PDH API를 사용하여 시스템 성능모니터 정보를 읽어오는 클래스
    
ProFiler
    
    성능 프로파일링을 위한 클래스
    
StreamBuffer
    
    직렬화버퍼 클래스
    typedef Packet; 하여 패킷으로 사용중
    
StreamQueue
    
    직렬화 링 버퍼 클래스



작업중

    GDIColor 
    BMPLoader
    DoubleBuffer
    ScreenDIB
    SpriteDIB


----------------------------------------------------------------
PathFind

    A-Star
    JumpPointSearch
----------------------------------------------------------------

----------------------------------------------------------------
tcpfighter

    client - winapi
    server - select모델
    dummy - 테스트용 더미
----------------------------------------------------------------

----------------------------------------------------------------
chatserver, loginserver

    Lib의 클래스들을 사용하여 제작한 채팅,로그인서버
----------------------------------------------------------------
