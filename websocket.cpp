#include "websocket.h"
#define MAX_TOKENS 10
#define MAX_IDS 50
#define LED 2

char eventIds[MAX_IDS][50];
int currentIdCount = 0; 

void onArrayNonEmpty() {
    // Some example procedures showing how to display to the pixels:
  blinkBlueEvent(5, 500);
  delay(500);
  displayHourOnNeoPixel();
  // colorWipe(strip.Color(0, 255, 0), 50); // Green
  // colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // // Send a theater pixel chase in...
  // theaterChase(strip.Color(127, 127, 127), 50); // White
  // theaterChase(strip.Color(127, 0, 0), 50); // Red
  // theaterChase(strip.Color(0, 0, 127), 50); // Blue

  // rainbow(20);
  // rainbowCycle(20);
  // theaterChaseRainbow(50);
}

void onArrayEmpty() {
  turn_off();
  clearLedState();
  theaterChaseRainbow(5);
  displayHourOnNeoPixel();
}

void connect_websocket(WebSocketsClient &ws, const char* ip_host, const uint16_t port) {
  ws.begin(ip_host, port);  // Connect to the WebSocket server
}

int splitString(const char* str, const char* delimiter, char tokens[MAX_TOKENS][50]) {
    int tokenIndex = 0;
    char* tempStr = strdup(str); 
    char* token = strtok(tempStr, delimiter);
    
    while (token != NULL && tokenIndex < MAX_TOKENS) {
        strncpy(tokens[tokenIndex], token, 50); 
        tokenIndex++;
        token = strtok(NULL, delimiter); 
    }
    
    free(tempStr); 
    return tokenIndex; 
}

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void handleEventUpdate(char* eventText) {
  Serial.printf("\n %s", eventText);
  clearLedState();
  char tokens[MAX_TOKENS][50];
    
  int numTokens = splitString(eventText, " - ", tokens);
 Serial.printf("\n num token : %d", numTokens);
  if(numTokens > 1) {
    for (int i = 1; i < numTokens; i++) {
        Serial.printf("\nToken %d: %s", i, tokens[i]);

        // Convert the token string to an integer
        uint8_t digit = atoi(tokens[i]);  // Convert string to integer
        
        displayLedBaseOnHour(digit);  
    }
  }
}

void handleEventOccur(char* eventText) {
  char tokens[MAX_TOKENS][50];
    
  int numTokens = splitString(eventText, " - ", tokens);
  char* id = tokens[1];
    // Add the ID to the global array if there's space
    if (currentIdCount < MAX_IDS) {
        strncpy(eventIds[currentIdCount], id, 50);
        currentIdCount++;

        if (currentIdCount > 0) {
            onArrayNonEmpty();  // Call function when array has at least one ID
        }
    } else {
        Serial.println("Error: ID array is full");
    }
}

void handleEventEnd(char* eventText) {
  char tokens[MAX_TOKENS][50];
    
  int numTokens = splitString(eventText, " - ", tokens);
  char* id = tokens[1];

  Serial.printf("id: %s", id);

    int idIndex = -1;
    for (int i = 0; i < currentIdCount; i++) {
        if (strcmp(eventIds[i], id) == 0) {
            idIndex = i;
            break;
        }
    }
    
    // If the ID was found, shift the remaining IDs
    if (idIndex != -1) {
        for (int i = idIndex; i < currentIdCount - 1; i++) {
            strcpy(eventIds[i], eventIds[i + 1]);
        }
        currentIdCount--;  // Decrement the count of stored IDs

        if (currentIdCount == 0) {
            onArrayEmpty();  // Call function when array is empty
        }
    } else {
        Serial.println("Error: ID not found");
    }displayHourOnNeoPixel();
}

void websocket_evt_handler(WStype_t type, uint8_t * payload, size_t length) {
   switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to URL: %s\n", payload);
      break;
    case WStype_TEXT: {
      char* textReceived = (char*)payload;
      handleEventUpdate(textReceived);

      if (prefix("EVENT_OCCUR", textReceived)) {
          handleEventOccur(textReceived);
      } else if (prefix("EVENT_END", textReceived)) {
         handleEventEnd(textReceived);
      }

      break;
    }
    case WStype_BIN:
      char* dataReceived = (char*)payload;

      Serial.printf("[WSc] Received binary length: %s\n", dataReceived);
      Serial.printf("Payload: %s\n", payload);

      if(strcmp(dataReceived, "LED_ON") == 0) {
        digitalWrite(LED, LOW);  // Turn LED on
        Serial.println("LED turned ON");
      } else if (strcmp(dataReceived, "LED_OFF") == 0) {
        digitalWrite(LED, HIGH);  // Turn LED off
        Serial.println("LED turned OFF");
      } else {
        Serial.println("UNKNOWN COMMAND");
      }
      break;
  }
}


void handle_websocket_evt(WebSocketsClient &ws) {
  ws.onEvent(websocket_evt_handler);
}