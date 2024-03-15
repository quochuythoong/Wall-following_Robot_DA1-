#define DELAY_TIME 200 // milliseconds

volatile unsigned long currentTime = 0; // Stores the current time in milliseconds

void setup() {
  Serial.begin(9600);
  TCCR1A = 0; // Set timer 1 control register A to 0 (normal mode)
  TCCR1B = 0; // Set timer 1 control register B to 0 (reset)
  TCNT1 = 0; // Set timer 1 counter to 0
  
  // Set prescaler for 16 MHz clock: divide by 64 for ~250 kHz overflow rate
  TCCR1B |= (1 << CS11) | (1 << CS10);
  
  // Set interrupt on overflow
  TIMSK1 |= (1 << TOIE1);

  sei(); // Enable interrupts
}

void loop() {
  // This loop will be interrupted every 200 milliseconds
  // You can add code here to be executed periodically
  // Ex: Serial.println("Interrupt triggered!");
}

// Interrupt service routine
void timer1_overflow_interrupt() {
  currentTime = millis(); // Update current time using millis()
}

// Function to check if 200 milliseconds have passed since the last interrupt
bool isDelayOver() {
  unsigned long elapsedTime = currentTime - millis();
  // Handle potential overflow by checking if elapsedTime is negative
  return (elapsedTime >= DELAY_TIME) || (elapsedTime < 0);
}
