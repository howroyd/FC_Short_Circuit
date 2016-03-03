#define num_cells 1

const int out[num_cells]  = {
  12
}; // Output pins in order

const int v_in[num_cells]  = {
  A0
}; // Voltage input pins in order
const int en[num_cells]   = {
  9
}; // Manual switch input pins in order

unsigned long duration[num_cells] = {
  250
}; // Short circuit duration in ms in order
unsigned long period[num_cells] = {
  10
}; // Short circuit period in sec in order


class My_cell {
  public:
    My_cell(void);
    bool get_state();
    int set_fc(const int pin);
    void run(const bool pause);
    unsigned long set_period(const unsigned long period);
    unsigned long set_duration(const unsigned long duration);
    int set_switch(const int pin);
    int set_pin_v(const int pin);
    float* get_v();
  protected:
    bool set(bool state);
    float read_v();
  private:
    bool my_state;
    int my_fc;
    int my_switch;
    int pin_v;
    float v[3] = {0, 0, 0};
    unsigned long period;
    unsigned long duration;
    unsigned long time_last;
};

My_cell   FC[num_cells];

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting!");
  // put your setup code here, to run once:
  for (int i = 0 ; i < num_cells ; i++) {
    FC[i].set_fc(out[i]);
    FC[i].set_duration(duration[i]);
    FC[i].set_period(period[i]);
    FC[i].set_switch(en[i]);
    FC[i].set_pin_v(v_in[i]);
  }
  Serial.println("Go!");
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
  String out = String(millis());
  for (int i = 0 ; i < num_cells ; i++) {
    FC[i].run(false);
    out = String(out + '\t');
    out = String(out + "Ch" + i + '\t' + (FC[i].get_state() ? "SHORT" : " OPEN") + '\t' + FC[i].get_v()[0] + '\t' + FC[i].get_v()[1] + '\t' + FC[i].get_v()[2]);
  }
  Serial.println(out);
  Serial.flush();
}

void My_cell::run(const bool pause) {
  // Update sensors
  My_cell::v[0] = My_cell::read_v();
  My_cell::v[1] = min(My_cell::v[0], My_cell::v[1]);
  My_cell::v[2] = max(My_cell::v[0], My_cell::v[2]);

  // Process switching timers
  if (!digitalRead(My_cell::my_switch)) My_cell::set(false);
  else {
    unsigned long delta = millis() - My_cell::time_last;
    if (My_cell::my_state && (delta >= My_cell::duration)) {
      My_cell::set(false);
      My_cell::time_last = millis();
      My_cell::v[1] = My_cell::v[0]; My_cell::v[2] = My_cell::v[0];
    }
    else if (!My_cell::my_state && (delta >= My_cell::period)) {
      My_cell::set(true);
      My_cell::time_last = millis();
      My_cell::v[1] = My_cell::v[0]; My_cell::v[2] = My_cell::v[0];
    }
  }
  return;
}

bool My_cell::set(bool state) {
  digitalWrite(My_cell::my_fc, state);
  My_cell::my_state = digitalRead(My_cell::my_fc);
  if (My_cell::my_state == state) return state;
  else return My_cell::set(state); // RECURSION
}

unsigned long My_cell::set_duration(const unsigned long duration) {
  My_cell::duration = duration;
  return My_cell::duration;
}

unsigned long My_cell::set_period(const unsigned long period) {
  My_cell::period = period * 1000;
  return My_cell::period;
}

int My_cell::set_switch(const int pin) {
  pinMode(pin, OUTPUT);
  My_cell::my_switch = pin;
  return My_cell::my_switch;
}

int My_cell::set_fc(const int pin) {
  pinMode(pin, OUTPUT);
  My_cell::my_fc = pin;
  return My_cell::my_fc;
}

int My_cell::set_pin_v(const int pin) {
  pinMode(pin, INPUT);
  My_cell::pin_v = pin;
  return My_cell::pin_v;
}

float* My_cell::get_v() {
  return My_cell::v;
}

float My_cell::read_v() {
  return (float)analogRead(My_cell::pin_v) * 5.0 / 1023.0;
}

bool My_cell::get_state() {
  return My_cell::my_state;
}

My_cell::My_cell(void) {
  My_cell::my_fc = 0;
  My_cell::my_switch = 0;
  My_cell::my_state = false;
  My_cell::period = 99999;
  My_cell::duration = 0;
  My_cell::time_last = 0;
}






