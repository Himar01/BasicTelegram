#include "WiFiClientSecure.h"
#include "time.h"
#include "string"
#include "BasicTelegram.h"

const char* server = "api.telegram.org";
const int port = 443;
const char TELEGRAM_CERTIFICATE_ROOT[] = R"CERT(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz
NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE
AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw
DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD
E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH
/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy
DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh
GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR
tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA
AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE
FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX
WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu
9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr
gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo
2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO
LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI
4uJEvlz36hz1
-----END CERTIFICATE-----
)CERT";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure wificlientsecure;

void setClock();
char* string2charArray(std::string string);

/*
  This function creates an instance of the TelegramClient. It creates
  a new token with url encoding format. It specifies SSL certifies for
  telegram API and sets the date for date validation checking on WifiSecureClient.
*/
void TelegramClient::setup(char* token){
  // Token URL encode safe
  std::string token_str = "";
  token_str += token;
  int tokenColon = token_str.find(":");
  if( tokenColon != -1){
      std::string start = token_str.substr(0,tokenColon);
      std::string end = token_str.substr(tokenColon+1, token_str.length());
      token_str = start;
      token_str += "%3A";
      token_str += end;
      _token = string2charArray(token_str);
  } else{
      _token = token;
  }

  // Sets Telegram SSL root cert
  wificlientsecure.setTrustAnchors(&cert);

  setClock();
}

bool TelegramClient::sendMessage(int chatId, char* message ){
  int conn;
  // Creates a new HTTPS connection
  conn = wificlientsecure.connect(server,port);
   if (conn == 1) {
    // Creating POST request parameters
    std::string body_str = "{\"chat_id\":\""+std::to_string(chatId)+"\",\"text\":\"";
    body_str += message;
    body_str += "\"}";
    String body = string2charArray(body_str);
    std::string post_str = "POST /bot";
    post_str += _token;
    post_str += "/sendMessage HTTP/1.1";
    String post = string2charArray(post_str);
    int body_len = body.length();

      //Request
      wificlientsecure.println(post);
      //Headers
      wificlientsecure.print("Host: "); wificlientsecure.println(server);
      wificlientsecure.println("Content-Type: application/json");
      wificlientsecure.print("Content-Length: "); wificlientsecure.println(body_len);
      wificlientsecure.println("Connection: Close");
      wificlientsecure.println();
      //Body
      wificlientsecure.println(body);
      wificlientsecure.println();

      //Wait for server response
      while (wificlientsecure.available() == 0);

      //Check server response
      String wificlientsecure_response = wificlientsecure.readStringUntil('\n');
      // Gets only "200 OK" text of the response.
      String state;
      for(int i = 0; i<6; i++ ){
        state += wificlientsecure_response[i+9]; 
      }
      if(state == "200 OK"){
        wificlientsecure.stop();
        return true;
      } else{
        wificlientsecure.stop();
        return false;
      }
   } else {
      wificlientsecure.stop();
      return false;
   }
}

// This function sets the actual time and date for cert date validation only
void setClock(){
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    now = time(nullptr);
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
}

// It allows to convert from strings to char arrays.
char* string2charArray(std::string string){
    char* cstr = new char[string.length() +1];
    strcpy(cstr, string.c_str());
    return cstr; 
}