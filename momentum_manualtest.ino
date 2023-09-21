const int high = 2000; // PWMの上限[us]
const int low = 1000; // PWMの下限[us]

const int channel = 0; // LEDCチャンネル
const int resolution = 12; // PWMの分解能（12ビット）
const int frequency = 50; // PWMの周波数（通常、50Hz）

int level = 0; // resolutionに対する割合
const int pin = 22; // ESCへの出力ピン

int min_level = 213;
int max_level = (1 << resolution)*2/20;

void setup(){
  Serial.begin(9600);
  while(!Serial){  //シリアルモニタが準備できるまで待機
  }
  // ledcSetup(チャンネル番号, PWM周波数, PWM解像度（8, 10, 12, 15ビットのいずれか））
  ledcSetup(channel, frequency, resolution);
  ledcAttachPin(pin, channel);
  
  // 待機状態
  Serial.println("Enter something to start ESC.");
  while (!Serial.available());  //シリアルポートで何か入力されるまで待ちます
  Serial.read();
  
  // 起動指令
  level = 220;
  ledcWrite(channel, level);
  delay(1000);
  level = 200;
  ledcWrite(channel, level);
  
  Serial.println("ESC started.");
}

void loop() {
  int percent = readNumberFromSerial();
  if((percent >= 0) && (percent <=100)){
    Serial.print("pecent : ");
    Serial.println(percent);
    
    level = map(ESC_power(percent), low, high, min_level, max_level);
    ledcWrite(channel, level);

    Serial.print("level : ");
    Serial.println(level);
  }
  delay(100);
}

int ESC_power(int percent){
  return low + percent*10;
}

int readNumberFromSerial() {
  int number = 0;
  unsigned long startTime = millis(); // 開始時間を記録
  bool isNegative = false; // 負の値かどうかを示すフラグ
  bool hasValue = false; // 数値が読み取られたかを示すフラグ

  while (millis() - startTime < 1000) { // 最大で1秒間、数字を待つ
    if (Serial.available() > 0) {
      char incomingChar = Serial.read(); // 1文字読み取り
      if (incomingChar == '-') { // マイナス符号の場合
        isNegative = true;
      } else if (isdigit(incomingChar)) { // 文字が数字かどうかを確認
        number = number * 10 + (incomingChar - '0'); // 数字を組み立てる
        hasValue = true; // 数値が読み取られたことを示す
      } else if (incomingChar == '\n') { // 改行が来たら数字の終わりとみなす
        if (hasValue) {
          return isNegative ? -number : number; // 数値があれば正または負に変換して返す
        } else {
          return -9999; // エラーコード（数字がまだ読み取られていない）
        }
      }
    }
  }

  if (hasValue) {
    return isNegative ? -number : number; // タイムアウトしても数字があれば正または負に変換して返す
  } else {
    return -9999; // エラーコード（数字がまだ読み取られていない）
  }
}