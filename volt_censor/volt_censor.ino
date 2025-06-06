// Calibration values obtained from the sketch: volt_ac_cal
const int adc_max = 760;                // Maximum sensor value during calibration
const int adc_min = 261;                // Minimum sensor value during calibration

const float volt_multi = 231;           // RMS voltage obtained from a multimeter
float volt_multi_p;                     // Peak voltage
float volt_multi_n;                     // Negative peak voltage

void setup() {
  Serial.begin(115200);

  volt_multi_p = volt_multi * 1.4142;   // Peak voltage = RMS voltage * 1.4142 (Single-phase current)
  volt_multi_n = -volt_multi_p;         // Negative peak voltage
  
  // Initial serial output to check calibration
  Serial.println("Starting Voltage Measurement...");
}

void loop() {
  float volt_rms = get_voltage();       // Root Mean Square voltage (V-RMS)

  Serial.print("Vrms: ");
  Serial.print(volt_rms, 3);
  Serial.println(" VAC");

  delay(1000); // Delay for readability
}

float get_voltage() {
  float adc_sample;
  float volt_inst = 0;
  float sum = 0;
  float volt;
  long init_time = millis();
  int N = 0;

  // Accumulate the sum of squared voltages over 0.5 seconds
  while ((millis() - init_time) < 500) {   // Duration of 0.5 seconds (Approximately 30 cycles of 60Hz)
    adc_sample = analogRead(A0);            // Read sensor voltage
    // Map the ADC value to the corresponding voltage
    volt_inst = map(adc_sample, adc_min, adc_max, volt_multi_n, volt_multi_p);
    // Accumulate the square of the instantaneous voltage
    sum += sq(volt_inst);                    
    N++;
    delay(1);
  }

  // Ensure N is greater than 0 to prevent division by zero
  if (N > 0) {
    volt = sqrt(sum / N);                     // Calculate RMS voltage
  } else {
    volt = 0; // Default to 0 if no readings were accumulated
  }
  
  return volt;
}
