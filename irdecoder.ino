#define LED_PIN 3
#define IR_PIN 4

#define OFF_INDEX 0
#define ON_INDEX 1

// Delay between reads in microseconds
#define USEC_DELAY 20 

// Max time to wait for a pulse
#define MAX_PULSE_TIME 65000

#define PYTHON_PRINT 1

#define MAX_CODES 150

int num_pulses = 0;
unsigned int pulses[MAX_CODES][2];

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  if (PYTHON_PRINT) {
    //Serial.println("READY");
  } else {
    Serial.println("Ready to accept IR");
  }
}

void loop(void) {
  unsigned int off_time = 0;
  unsigned int on_time = 0;

  // read an off pulse
  while (PIND & (1 << IR_PIN)) {
    // IR pin is high; therefore off
    off_time += 1;
    delayMicroseconds(USEC_DELAY);

    if (off_time >= MAX_PULSE_TIME && num_pulses != 0) {
      // Max waiting time reached; Signal done
      signalDone();
      return;
    }
  }
  if (num_pulses >= MAX_CODES) {
    Serial.println("Too many pulses");
    num_pulses = 0;
    return;
  }
  pulses[num_pulses][OFF_INDEX] = off_time;

  // read the on pulses
  while (!(PIND & (1 << IR_PIN))) {
    // IR pin is low; therefore on
    on_time++;
    delayMicroseconds(USEC_DELAY);

    if (on_time >= MAX_PULSE_TIME && num_pulses != 0) {
      // Max waiting time reached; Signal done
      signalDone();
      return;
    }
  }
  pulses[num_pulses][ON_INDEX] = on_time;

  num_pulses += 1;
}

void pythonPrint() {
  for (int i = 0; i < num_pulses; i += 1) {
    Serial.print(pulses[i][OFF_INDEX]*USEC_DELAY, DEC);
    Serial.print(" ");
    Serial.print(pulses[i][ON_INDEX]*USEC_DELAY, DEC);
    Serial.print('\n');
  }
  Serial.print("END\n");
}

void humanPrint() {
  Serial.println("IR Sequence:");
  for (int i = 0; i < num_pulses; i += 1) {
    Serial.print("On: ");
    Serial.print(pulses[i][OFF_INDEX]*USEC_DELAY, DEC);
    Serial.print(" Off: ");
    Serial.print(pulses[i][ON_INDEX]*USEC_DELAY, DEC);
    Serial.print("\n");
  }

  Serial.print("Pulses: ");
  Serial.print(num_pulses, DEC);
  Serial.print("\n");
}

void signalDone(void) {
  if (PYTHON_PRINT) {
    pythonPrint();
  } else {
    humanPrint();
  }

  // Signal done; Pulses back to 0
  num_pulses = 0;
}
