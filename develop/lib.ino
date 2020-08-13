#include <SPI.h>
#include <SD.h>

const PROGMEM float coeffs[] = {1.76384252869671, 1.7154572200005442, 1.6351696263031674, 1.5766928552532127, 
                                1.3063456859075537, 1.191398608984867, 0.9857963844595683, 0.8104826280099796};
const PROGMEM float black[] = {
0.006878018379211426, 0.006935006483945974, 0.007106103713363648, 0.007391708330140939, 0.007792482496781619,
0.00830934986709432, 0.00894349222307339, 0.009696345165972634, 0.01056959287283113, 0.011565161932160159,
0.01268521427490911, 0.013932139219203735, 0.015308544649676376, 0.016817247354482902, 0.018461262545322876,
0.020243792587934212, 0.02216821497262504, 0.024238069556421507, 0.02645704511035033, 0.0288289652072306,
0.03135777348712121, 0.03404751833924685, 0.036902337040809766, 0.0399264393945777, 0.043124090908518736,
0.04649959556202746, 0.050057278204451744, 0.05380146663268032, 0.05773647339548663, 0.06186657737314577,
0.06619600518153684, 0.07072891245052318, 0.07546936502685536, 0.08042132015217372, 0.0855886076668918,
0.09097491129082115, 0.09658375003135286, 0.10241845976983843, 0.10848217507651484, 0.11477781130389673,
0.12130804700801177, 0.128075306746186, 0.1350817442992961, 0.14232922636549503, 0.14981931677139196,
0.15755326124552416, 0.16553197279770526, 0.1737560177464731, 0.18222560243538719, 0.1909405606773603,
0.1999003419645327, 0.20910400047943747, 0.21855018494134748, 0.2282371293197531, 0.2381626444448992,
0.24832411054320663, 0.25871847072323495, 0.2693422254356059, 0.28019142792800555, 0.29126168071403125,
0.3025481330722449, 0.31404547958934503, 0.3257479597588838, 0.3376493586444366, 0.34974300861358637,
0.36202179214651986, 0.3744781457204527, 0.38710406476851533, 0.3998911097091411, 0.41283041303941553,
0.4259126874832729, 0.43912823518286914, 0.45246695791893, 0.46591836834336936, 0.4794716022050044,
0.49311543154676896, 0.5068382788504501, 0.5206282321026424, 0.5344730607533511, 0.5483602325364739,
0.5622769311192501, 0.5762100745457207, 0.5901463344372482, 0.6040721559112647, 0.6179737781776001,
0.6318372557700426, 0.6456484803691538, 0.6593932031708652, 0.6730570577539623, 0.6866255833982696,
0.7000842488041658, 0.7134184761629803, 0.7266136655268723, 0.7396552194259599, 0.7525285676797484,
0.7652191923493276, 0.7777126527763367, 0.7899946106543597, 0.8020508550782071, 0.8138673275164574,
0.8254301466526721, 0.8367256330408851, 0.8477403335212518, 0.8584610453421903, 0.8688748399358914,
0.8789690862947551, 0.8887314738971245, 0.8981500351315997, 0.9072131671702709, 0.9159096532423648,
0.9242286832610802, 0.9321598737577689, 0.9396932870791218, 0.9468194498046159, 0.9535293703431773,
0.9598145556698215, 0.9656670271649104, 0.9710793355206584, 0.9760445746815715, 0.9805563947876479,
0.9846090140913881, 0.9881972298219366, 0.9913164279720323, 0.9939625919858431, 0.9961323103282166,
0.9978227829183829, 0.999031826413684, 0.9997578783314818, 1.0, 0.9997578783314818,
0.999031826413684, 0.9978227829183829, 0.9961323103282167, 0.9939625919858431, 0.9913164279720325,
0.9881972298219366, 0.9846090140913881, 0.9805563947876479, 0.9760445746815715, 0.9710793355206585,
0.9656670271649104, 0.9598145556698215, 0.9535293703431773, 0.9468194498046159, 0.9396932870791218,
0.9321598737577689, 0.9242286832610804, 0.9159096532423648, 0.9072131671702709, 0.8981500351315997,
0.8887314738971245, 0.8789690862947551, 0.8688748399358914, 0.8584610453421905, 0.8477403335212519,
0.8367256330408852, 0.8254301466526722, 0.8138673275164574, 0.8020508550782071, 0.7899946106543597,
0.7777126527763368, 0.7652191923493278, 0.7525285676797486, 0.7396552194259604, 0.7266136655268729,
0.7134184761629807, 0.7000842488041658, 0.6866255833982696, 0.6730570577539624, 0.6593932031708655,
0.6456484803691539, 0.6318372557700427, 0.6179737781776002, 0.6040721559112648, 0.5901463344372486,
0.5762100745457212, 0.5622769311192506, 0.5483602325364738, 0.5344730607533511, 0.5206282321026424,
0.5068382788504502, 0.49311543154676907, 0.4794716022050045, 0.46591836834336964, 0.4524669579189302,
0.43912823518286936, 0.42591268748327327, 0.41283041303941537, 0.399891109709141, 0.38710406476851533,
0.3744781457204527, 0.36202179214651997, 0.34974300861358654, 0.3376493586444368, 0.32574795975888404,
0.31404547958934526, 0.30254813307224515, 0.2912616807140315, 0.2801914279280055, 0.26934222543560593,
0.25871847072323495, 0.24832411054320663, 0.23816264444489932, 0.22823712931975318, 0.21855018494134754,
0.20910400047943767, 0.19990034196453288, 0.1909405606773605, 0.1822256024353871, 0.173756017746473,
0.16553197279770523, 0.15755326124552416, 0.14981931677139204, 0.1423292263654951, 0.13508174429929623,
0.12807530674618617, 0.12130804700801193, 0.11477781130389693, 0.10848217507651507, 0.10241845976983847,
0.09658375003135287, 0.0909749112908212, 0.08558860766689186, 0.08042132015217375, 0.07546936502685539,
0.07072891245052329, 0.066196005181537, 0.06186657737314586, 0.057736473395486716, 0.05380146663268044,
0.050057278204451716, 0.04649959556202747, 0.04312409090851873, 0.03992643939457768, 0.036902337040809786,
0.03404751833924687, 0.03135777348712121, 0.028828965207230647, 0.02645704511035038, 0.02423806955642155,
0.022168214972625076, 0.02024379258793422, 0.018461262545322883, 0.016817247354482902, 0.015308544649676362,
0.013932139219203707, 0.012685214274909082, 0.011565161932160187, 0.010569592872831171, 0.009696345165972661,
0.008943492223073418, 0.008309349867094334, 0.007792482496781619, 0.007391708330140939, 0.0071061037133636346,
0.006935006483945974};

Sd2Card card;

volatile uint8_t sample;
volatile byte subTick;
volatile uint16_t playHead;
volatile uint32_t block;
volatile int blocks;

int hook = 0;

void lib_setup() {
  TCCR2A = 0b00000001;
  TCCR2B = 0b00000100;
  TIMSK2 = 0b00000000;
  OCR2B = 0;

  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(A0,INPUT);

  delay(3000);

  if(!card.init(SPI_HALF_SPEED, 10)) {
    byte state = HIGH;
    for (byte i = 0; i < 6; i++) {
      digitalWrite(2,state);
      if (state == HIGH) { state = LOW; } else { state = HIGH; };
      delay(250);
    }
  }
  card.partialBlockRead(1);

  //calibrate on hook voltage
  digitalWrite(2,LOW);
  delay(1000);
  hook = analogRead(A0);
  hook = 0.9*hook;
  digitalWrite(2,HIGH);
  #if PHONE == 1
  while(analogRead(A0) > hook) {};
  #endif
  digitalWrite(2,LOW);
}

int dtmf() {
  int cur, num, r, c, buf[256];
  float mass, q0[8], q1[8], q2[8];
  byte last;

  #if CONSOLE == 1
  char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  #endif

  int nums[4][4] = {
    { 1, 2, 3,10},
    { 4, 5, 6,11},
    { 7, 8, 9,12},
    {14, 0,15,13}
  };

  num = 0;
  last = 0;
  while(1) {
    //bail if receiver is on the hook
    #if PHONE == 1
    if (analogRead(A0) > hook) {
      digitalWrite(2,HIGH);
      while(analogRead(A0) > hook) {};
      digitalWrite(2,LOW);
      return -1;
    }
    #else
    return -1;
    #endif

    // ~8887Hz
    for (int i = 0; i < 256; i++) {
      buf[i] = analogRead(A0);
    }

    memset(q0,0,8*sizeof(float));
    memset(q1,0,8*sizeof(float));
    memset(q2,0,8*sizeof(float));

    for (int n = 0; n < 256; n++) {
      float s = buf[n] * pgm_read_float_near(black + n);

      for (byte i = 0; i < 8; i++) {
        q0[i] = pgm_read_float_near(coeffs + i)*q1[i]-q2[i]+s;
        q2[i] = q1[i];
        q1[i] = q0[i];
      }

      for (byte i = 0; i < 8; i++) {
        q0[i] = sqrt(q1[i]*q1[i]+q2[i]*q2[i]-q1[i]*q2[i]*pgm_read_float_near(coeffs + i));
      }
    }

    mass = 0;
    for (byte j = 0; j < 8; j++) mass = mass + q0[j];

    r = -1;
    c = -1;
    for (byte i = 0; i < 8; i++) {
      if (q0[i]/mass > 0.3) {
        if (i < 4) {
          r = i;
        } else {
          c = i - 4;
        }
      }
    }

    if (r != -1 && c != -1) {
      digitalWrite(2,HIGH);

      if (last == 0) {
        last = 1;
        cur = nums[r][c];
        switch(cur) {
          case 15:
            last = 15;
            break;
          case 14:
            num = 0;
            break;
          default:
            num = num * 10;
            num = num + cur;
        }
      }

      #if CONSOLE == 1
      Serial.print(keys[r][c]);
      #endif
    } else {
      digitalWrite(2,LOW);

      if (last == 15) {
        #if CONSOLE == 1
        Serial.print("\n");
        #endif
        return num;
      }
      last = 0;

      #if CONSOLE == 1
      Serial.print("."); 
      #endif
    }

  }

}

int play(int superblock, int blks) {
  block = superblock*8;
  blocks = blks;
  subTick = 0;
  playHead = 0;
  OCR2B = 0;
  TCCR2A = 0b00100011;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000001;
  while(blocks > 0) {
    if (analogRead(A0) > hook) {
      TCCR2A = 0b00000001;
      TCCR2B = 0b00000100;
      TIMSK2 = 0b00000000;
      OCR2B = 0;
      digitalWrite(2,HIGH);
      while(analogRead(A0) > hook) {};
      digitalWrite(2,LOW);
      return -1;
    }
  }
  TCCR2A = 0b00000001;
  TCCR2B = 0b00000100;
  TIMSK2 = 0b00000000;
  OCR2B = 0;
  return 1;
}


ISR(TIMER2_OVF_vect){
  if (blocks < 0) {
    OCR2B = 0;
    return;
  }
  OCR2B = sample;
  if (subTick++ < 7) return;
  subTick = 0;
  card.readData(block, playHead, 1, (uint8_t*) &sample);
  if (playHead++ < BLOCKSIZE) return;
  playHead = 0;
  block++;
  blocks--;
}
