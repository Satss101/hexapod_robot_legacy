#include <Servo.h>
#include <SpeedTrig.h>
#include <HardwareSerial.h>

HardwareSerial ftdi(PA10, PA9);
char keyInput;

#define servoDepanKanan_0 PB10
#define servoDepanKanan_1 PB1
#define servoDepanKanan_2 PB0
#define servoTengahKanan_0 PA7
#define servoTengahKanan_1 PA6
#define servoTengahKanan_2 PA5
#define servoBelakangKanan_0 PA0
#define servoBelakangKanan_1 PA1
#define servoBelakangKanan_2 PB6
#define servoBelakangKiri_0 PB7
#define servoBelakangKiri_1 PB8
#define servoBelakangKiri_2 PB9
#define servoTengahKiri_0 PA8
#define servoTengahKiri_1 PA3
#define servoTengahKiri_2 PA2
#define servoDepanKiri_0 PB13
#define servoDepanKiri_1 PB14
#define servoDepanKiri_2 PB15
const int pb[6] = { PC14, PA4, PC15, PA15, PC13, PB12 };
int tes[6];

Servo servo0_0;
Servo servo0_1;
Servo servo0_2;
Servo servo1_0;
Servo servo1_1;
Servo servo1_2;
Servo servo2_0;
Servo servo2_1;
Servo servo2_2;
Servo servo3_0;
Servo servo3_1;
Servo servo3_2;
Servo servo4_0;
Servo servo4_1;
Servo servo4_2;
Servo servo5_0;
Servo servo5_1;
Servo servo5_2;

#define coxa 25  //30 //waktu dibagi dengan
#define femur 55
#define tibia 112
#define rate 5            // ketelitian langkah  //harus ganjil
#define lebarLangkah 30   // lebar langkah dibagi rate*2 harus bulat 40 4 32 //harus bulat agar stabil
#define tinggiLangkah 30  //tinggi langkah dibagi rate harus bulat
#define delayKecepatan 80
int Zoff = -95;

int input = 0;
void syncLeg();
void inverse(int idLeg, float x, float y, float z);

float posisiAwal = ((coxa + femur) / 1);  //1.5
// float posisiKepiting = ((coxa + femur) / 1.3);
float langkahNaik = (tinggiLangkah / rate);
int jumlahPosisiKaki = ((rate * 2) + 1);  //terdapat pada void motion
int rate1 = (rate + 1);
int rate2 = ((rate * 2) + 1);
int rate3 = ((rate * 3) + 1);
int rate4 = ((rate * 4) + 1);
int indexKanan = 0;
int indexKiri = 0;
int inputBefore = 0;
int ubahGerak = 0;

float langkahDatar[20];
int bacaKakiPb[6];
int selisih[6];

struct sys {
  float pos, teta, tet;
  int sel, prev_teta, angle;
} sys[18];

struct leg {
  int sudutDalam, sudutTengah, sudutLuar, posisi, motion, gerakan;
  int tinggi, bukaan, putar;  // ada 4 motion
  float posisiX, posisiY, posisiZ;
  // 0 = gerakan segitiga     1 = gerakan dorong
} leg[6];

void inisialisasiAwal() {
  int posisiX = 90;
  int posisiY = 90;
  for (int a = 0; a < 6; a++) {
    if (a > 2) leg[a].posisiX = posisiX * -1;
    else leg[a].posisiX = posisiX;
    leg[0].posisiY = posisiY;
    leg[1].posisiY = 0;
    leg[2].posisiY = (posisiY * (-1));
    leg[3].posisiY = posisiY;
    leg[4].posisiY = 0;
    leg[5].posisiY = (posisiY * (-1));;
    leg[a].posisiZ = -10;
    leg[a].posisi = jumlahPosisiKaki;
    leg[a].gerakan = 1;
    inverse(a, leg[a].posisiX, leg[a].posisiY, leg[a].posisiZ);
  }
}

void setKaki() {
  if (inputBefore != 0) {
    leg[0].putar = -15;  //kurang ke kepan
    leg[1].putar = 18;   //tambah ke depan
    leg[2].putar = -15;  //dikurang semakin kedepan
    leg[3].putar = 10;
    leg[4].putar = 20;   //tambah ke depan
    leg[5].putar = -15;  //tambah ke depan

    //tambah nilai tambah naik
    leg[0].bukaan = 0;
    leg[1].tinggi = 0;
    leg[2].tinggi = 0;
    //kurang nilai tambah naik
    leg[3].tinggi = 0;
    leg[4].tinggi = 0;
    leg[5].tinggi = 0;

    //tambah nilai tambah buka
    leg[0].bukaan = 20;
    leg[1].bukaan = 20;
    leg[2].bukaan = 10;
    //kurang nilai tambah buka
    leg[3].bukaan = -30;
    leg[4].bukaan = -20;
    leg[5].bukaan = -30;
  } else {
    leg[0].putar = 0;  //kurang ke kepan
    leg[1].putar = 0;  //tambah ke depan
    leg[2].putar = 0;  //dikurang semakin kedepan
    //tambah nilai tambah maju
    leg[3].putar = 5;
    leg[4].putar = -5;  //tambah ke depan
    leg[5].putar = 5;   //tambah ke depan

    //tambah nilai tambah naik
    leg[0].tinggi = 0;
    leg[1].tinggi = 0;
    leg[2].tinggi = 0;
    //kurang nilai tambah naik
    leg[3].tinggi = 0;
    leg[4].tinggi = 0;
    leg[5].tinggi = 0;

    //tambah nilai tambah buka
    leg[0].bukaan = 20;
    leg[1].bukaan = 20;
    leg[2].bukaan = 10;
    //kurang nilai tambah buka
    leg[3].bukaan = -30;
    leg[4].bukaan = -20;
    leg[5].bukaan = -30;
  }
}

void setGerak() {
  langkahDatar[0] = (lebarLangkah / (rate * (2)));

  langkahDatar[1] = (lebarLangkah / (rate * (1)));  //ke arah 1 kanan
  langkahDatar[2] = (lebarLangkah / (rate * (1)));  //ke arah 1 kiri

  langkahDatar[3] = (lebarLangkah / (rate * (1.2)));  //ke arah 4 kanan
  langkahDatar[4] = (lebarLangkah / (rate * (1.5)));  //ke arah 4 kiri

  langkahDatar[5] = (lebarLangkah / (rate * (1)));  //ke arah 2 kanan
  langkahDatar[6] = (lebarLangkah / (rate * (1)));  //ke arah 2 kiri

  langkahDatar[7] = (lebarLangkah / (rate * (1.3)));  //ke arah 3 kanan
  langkahDatar[8] = (lebarLangkah / (rate * (1.5)));  //ke arah 3 kiri

  langkahDatar[9] = (lebarLangkah / (rate * (1)));  //pivot
}

void setup() {
  Serial.begin(115200);
  ftdi.begin(1000000);
  servo0_0.attach(servoDepanKanan_0);
  servo0_1.attach(servoDepanKanan_1);
  servo0_2.attach(servoDepanKanan_2);
  servo1_0.attach(servoTengahKanan_0);
  servo1_1.attach(servoTengahKanan_1);
  servo1_2.attach(servoTengahKanan_2);
  servo2_0.attach(servoBelakangKanan_0);
  servo2_1.attach(servoBelakangKanan_1);
  servo2_2.attach(servoBelakangKanan_2);
  servo3_0.attach(servoBelakangKiri_0);
  servo3_1.attach(servoBelakangKiri_1);
  servo3_2.attach(servoBelakangKiri_2);
  servo4_0.attach(servoTengahKiri_0);
  servo4_1.attach(servoTengahKiri_1);
  servo4_2.attach(servoTengahKiri_2);
  servo5_0.attach(servoDepanKiri_0);
  servo5_1.attach(servoDepanKiri_1);
  servo5_2.attach(servoDepanKiri_2);
  for (int i = 0; i < 6; i++) {
    pinMode(pb[i], INPUT_PULLUP);
  }
  setKaki();
  setGerak();
  inisialisasiAwal();
  syncLeg();
}

long rad2deg(float rad) {
  return ((int)(rad * 57.29577957855));
}

float gerakServo(float nilai) {
  return map(nilai, 0, 180, 700, 1950);
}

int float2int(float input) {
  int temp = (int)input;            // floor-rounded input
  float dif = input - (float)temp;  //
  if (dif < 0.5) return temp;
  else return temp + 1;
}

void bacaPb() {
  for (int j = 0; j < 6; j++) {
    bacaKakiPb[j] = digitalRead(pb[j]);
    // if (j == 0) {
    //   Serial.println(bacaKakiPb[j]);
    // }
  }
}

void syncWrite() {
  for (int ang = 0; ang < 18; ang++) {
    sys[ang].prev_teta = sys[ang].teta;
    sys[ang].teta = sys[ang].angle;
    sys[ang].sel = sys[ang].teta - sys[ang].prev_teta;
    if (sys[ang].sel < 0) sys[ang].sel = sys[ang].sel * (-1);
  }
  int min_s = min(sys[0].sel, sys[1].sel);
  for (int i = 2; i < 18; i++) {
    min_s = min(min_s, sys[i].sel);
  }
  if (min_s == 0) min_s = 1;
  for (int i = 0; i <= min_s; i++) {
    for (int ang = 0; ang < 18; ang++) {
      sys[ang].tet = sys[ang].prev_teta + (((sys[ang].teta - sys[ang].prev_teta) / min_s) * i);
    }
    servo0_0.writeMicroseconds(gerakServo(sys[0].tet));
    servo0_1.writeMicroseconds(gerakServo(sys[1].tet));
    servo0_2.writeMicroseconds(gerakServo(sys[2].tet));
    servo1_0.writeMicroseconds(gerakServo(sys[3].tet));
    servo1_1.writeMicroseconds(gerakServo(sys[4].tet));
    servo1_2.writeMicroseconds(gerakServo(sys[5].tet));
    servo2_0.writeMicroseconds(gerakServo(sys[6].tet));
    servo2_1.writeMicroseconds(gerakServo(sys[7].tet));
    servo2_2.writeMicroseconds(gerakServo(sys[8].tet));

    servo3_0.writeMicroseconds(gerakServo(sys[9].tet));
    servo3_1.writeMicroseconds(gerakServo(sys[10].tet));
    servo3_2.writeMicroseconds(gerakServo(sys[11].tet));
    servo4_0.writeMicroseconds(gerakServo(sys[12].tet));
    servo4_1.writeMicroseconds(gerakServo(sys[13].tet));
    servo4_2.writeMicroseconds(gerakServo(sys[14].tet));
    servo5_0.writeMicroseconds(gerakServo(sys[15].tet));
    servo5_1.writeMicroseconds(gerakServo(sys[16].tet));
    servo5_2.writeMicroseconds(gerakServo(sys[17].tet));
    delay(delayKecepatan);
  }
}

void setServo(int idLeg, int sudut1, int sudut2, int sudut3) {
  if (idLeg == 0) {
    sys[0].angle = (90 - sudut1) + leg[idLeg].putar;
    sys[1].angle = (90 - sudut2) + leg[idLeg].tinggi;  //240
    sys[2].angle = (90 + sudut3) + leg[idLeg].bukaan;  //60
  } else if (idLeg == 1) {
    sys[3].angle = (90 - sudut1) + leg[idLeg].putar;
    sys[4].angle = (90 - sudut2) + leg[idLeg].tinggi;  //240
    sys[5].angle = (90 + sudut3) + leg[idLeg].bukaan;  //60
  } else if (idLeg == 2) {
    sys[6].angle = (90 - sudut1) + leg[idLeg].putar;
    sys[7].angle = (90 - sudut2) + leg[idLeg].tinggi;  //60
    sys[8].angle = (90 + sudut3) + leg[idLeg].bukaan;  //240
  } else if (idLeg == 3) {
    sys[9].angle = (270 - sudut1) + leg[idLeg].putar;
    sys[10].angle = (90 + sudut2) + leg[idLeg].tinggi;  //60
    sys[11].angle = (90 - sudut3) + leg[idLeg].bukaan;  //240
  } else if (idLeg == 4) {
    sys[12].angle = (270 - sudut1) + leg[idLeg].putar;
    sys[13].angle = (90 + sudut2) + leg[idLeg].tinggi;  //60
    sys[14].angle = (90 - sudut3) + leg[idLeg].bukaan;  //240
  } else if (idLeg == 5) {
    sys[15].angle = (270 - sudut1) + leg[idLeg].putar;
    sys[16].angle = (90 + sudut2) + leg[idLeg].tinggi;  //60
    sys[17].angle = (90 - sudut3) + leg[idLeg].bukaan;  //240
  }
}

void syncLeg() {
  setServo(0, leg[0].sudutDalam, leg[0].sudutTengah, leg[0].sudutLuar);
  setServo(1, leg[1].sudutDalam, leg[1].sudutTengah, leg[1].sudutLuar);
  setServo(2, leg[2].sudutDalam, leg[2].sudutTengah, leg[2].sudutLuar);
  setServo(3, leg[3].sudutDalam, leg[3].sudutTengah, leg[3].sudutLuar);
  setServo(4, leg[4].sudutDalam, leg[4].sudutTengah, leg[4].sudutLuar);
  setServo(5, leg[5].sudutDalam, leg[5].sudutTengah, leg[5].sudutLuar);
  syncWrite();
}

void inverse(int idLeg, float x, float y, float z) {
  float degree1, degree2, degree3;
  float degree2_1, degree2_2;
  float L1, L, L_2, A, Lcox;
  float temp2, temp2_2, temp2_3, temp2_4;
  float temp3, temp3_2, temp3_3, temp3_4;
  float femur_2, tibia_2;
  int sudut1, sudut2, sudut3;

  z = Zoff - z;
  degree1 = SpeedTrig.atan2(y, x);
  L1 = sqrt((x * x) + (y * y));  //akar x^2 + y^2

  Lcox = L1 - coxa;                   //coxa adalah 10
  L = sqrt((z * z) + (Lcox * Lcox));  //akar z^2 + Lcox^2
  femur_2 = femur * femur;
  tibia_2 = tibia * tibia;
  L_2 = L * L;

  degree2_1 = SpeedTrig.atan2(z, Lcox);
  temp2 = (((L_2 + femur_2) - tibia_2) / ((2 * femur) * L));
  degree2_2 = SpeedTrig.acos(temp2);
  // degree2_1 = degree2_1 * -1;
  degree2 = degree2_2 + degree2_1;

  temp3 = (((femur_2 + tibia_2) - L_2) / ((2 * femur) * tibia));
  degree3 = SpeedTrig.acos(temp3);

  sudut1 = float2int(rad2deg(degree1));
  sudut2 = float2int(rad2deg(degree2));
  sudut3 = float2int(rad2deg(degree3));

  if (L <= (femur + tibia)) sudut3 = sudut3 - 90;

  leg[idLeg].sudutDalam = sudut1;
  leg[idLeg].sudutTengah = sudut2;
  leg[idLeg].sudutLuar = sudut3;
}

void motion(int idLeg, int indexLebar)  //0,1
{
  if (leg[idLeg].motion == 0)  // ke Y positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi < rate1)  //(rate+1))
      {
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi++;
      } else if (leg[idLeg].posisi >= rate1 && leg[idLeg].posisi < rate2)  //leg[idLeg].posisi>=(rate+1) && leg[idLeg].posisi<((rate*2)+1)
      {
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate2) {
        leg[idLeg].gerakan = 1;
      }
    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi > 1) {
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 0;
      }
    }
  }

  else if (leg[idLeg].motion == 1)  // ke Y negatif 
  //gerakan sudah benar ke arah y negatif dan membentuk segitiga ke atas
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate1)  //(rate+1))
      {
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
        Serial.println("aaaaaaaaaaaaaaaa");
        bacaPb();
        if (bacaKakiPb[0] == 0) {
          tes[0] = 1;
          selisih[0] = rate1 - leg[idLeg].posisi;
          Serial.println(selisih[0]);
          leg[idLeg].posisi = 1;
          while (1) {};
        }
      } else if (leg[idLeg].posisi <= rate1)  //(rate+1))
      {
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
        Serial.println("bbbbbbb");
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisi++;
        Serial.println("c");
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  } else if (leg[idLeg].motion == 2)  // ke X negatif 
  //gerakan sudah benar mengarah ke x dan berbentuk segitiga ke atas
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi < rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi++;
      } else if (leg[idLeg].posisi >= rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate4)  //((rate*4)+1))
      {
        leg[idLeg].gerakan = 1;
      }
    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi > rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  }

  else if (leg[idLeg].motion == 3)  // ke X positif
  {
  //ke x negatif, gerakan sudah benar segitiga                 
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate4)  //((rate*4)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate4)  //((rate*4)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  }

  else if (leg[idLeg].motion == 4)  // ke X positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi < rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi++;
      } else if (leg[idLeg].posisi >= rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate4)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi > rate2)  //((rate*4)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*4)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  }

  else if (leg[idLeg].motion == 5)  // ke Y negatif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  } else if (leg[idLeg].motion == 6)  // ke Y positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  } else if (leg[idLeg].motion == 7)  // ke X positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate3)  //((rate*3)+1))
      {
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate4)  //((rate*4)+1))
      {
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate4)  //((rate*4)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  } else if (leg[idLeg].motion == 8)  // ke Y positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate1)  //(rate+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 1;
      }

    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  } else if (leg[idLeg].motion == 9)  // ke Y positif
  {
    if (leg[idLeg].gerakan == 0)  //gerak segitiga
    {
      if (leg[idLeg].posisi > rate1)  //(rate+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ -= langkahNaik;
        leg[idLeg].posisi--;
      } else if (leg[idLeg].posisi <= rate1)  //(rate+1))
      {
        leg[idLeg].posisiX -= langkahDatar[indexLebar];
        leg[idLeg].posisiY -= langkahDatar[indexLebar];
        leg[idLeg].posisiZ += langkahNaik;
        leg[idLeg].posisi--;
      }
      if (leg[idLeg].posisi == 1) {
        leg[idLeg].gerakan = 1;
      }
    } else if (leg[idLeg].gerakan == 1)  //dorong
    {
      if (leg[idLeg].posisi < rate2)  //((rate*2)+1))
      {
        leg[idLeg].posisiX += langkahDatar[indexLebar];
        leg[idLeg].posisiY += langkahDatar[indexLebar];
        leg[idLeg].posisi++;
      }
      if (leg[idLeg].posisi == rate2)  //((rate*2)+1))
      {
        leg[idLeg].gerakan = 0;
      }
    }
  }
  inverse(idLeg, leg[idLeg].posisiX, leg[idLeg].posisiY, leg[idLeg].posisiZ);
}

void directions() {
  if (inputBefore == 1) {
    indexKanan = 1;
    indexKiri = 2;
    leg[0].motion = 0;
    leg[1].motion = 0;
    leg[2].motion = 2;
    leg[3].motion = 4;
    leg[4].motion = 1;
    leg[5].motion = 4;
  } else if (inputBefore == 2) {
    indexKanan = 3;
    indexKiri = 4;
  } else if (inputBefore == 3) {
    indexKanan = 5;
    indexKiri = 6;
    leg[0].motion = 0;
    leg[1].motion = 2;
    leg[2].motion = 0;
    leg[3].motion = 5;
    leg[4].motion = 0;
    leg[5].motion = 5;
  } else if (inputBefore == 4) {  //KANAN
    indexKanan = 7;
    indexKiri = 8;
    leg[0].motion = 6;  //solved
    leg[1].motion = 6;
    leg[2].motion = 9;
    leg[3].motion = 6;
    leg[4].motion = 1;
    leg[5].motion = 6;
    // leg[idLeg].posisiX = posisiKepiting;
    // leg[idLeg].posisiY = posisiKepiting;
    // leg[idLeg].posisiZ = -20;
  } else if (inputBefore == 5) {  //pivot KIRI
    indexKanan = 9;
    indexKiri = 9;
    leg[0].motion = 1;
    leg[1].motion = 2;
    leg[2].motion = 1;
    leg[3].motion = 5;
    leg[4].motion = 0;
    leg[5].motion = 5;
  } else if (inputBefore == 6) {  //pivot KANAN
    indexKanan = 9;
    indexKiri = 9;
    leg[0].motion = 0;
    leg[1].motion = 7;  //3 sudah sesuai tap2nya
    leg[2].motion = 0;
    leg[3].motion = 4;
    leg[4].motion = 1;
    leg[5].motion = 4;
  }

  // Serial.println(ubahGerak);
  // if (ubahGerak == 0) {
  //   // Serial.println("ubahGerak");
  //   // setKaki();
  //   // syncLeg();
  //   while (leg[0].posisi != 1) {
  //     leg[0].motion = 1;
  //     motion(0, indexKanan);
  //     syncLeg();
  //   }
  //   while (leg[2].posisi != rate4)  //((rate*4)+1))
  //   {
  //     leg[2].motion = 1;
  //     motion(2, indexKanan);
  //     syncLeg();
  //   }
  //   ubahGerak = 1;
  // }

  //GERAKAN PADA PIVOT
  // if (ubahGerak == 0) {
  //   while (leg[0].posisi != rate4 && leg[2].posisi != rate4) {
  //     leg[0].motion = 2;
  //     motion(0, 0);
  //     leg[2].motion = 2;
  //     motion(2, 0);
  //     syncLeg();
  //   }
  //   ubahGerak = 1;
  // }
  motion(0, indexKanan);
  motion(1, indexKanan);
  motion(2, indexKanan);
  motion(3, indexKiri);
  motion(4, indexKiri);
  motion(5, indexKiri);
  syncLeg();
}

/*
// void pivot(int arah, int ubahGerak) {
  if (arah == 0)  //putar kiri
  {
    if (ubahGerak == 0) {
      while (leg[0].posisi != rate4 && leg[2].posisi != rate4) {
        leg[0].motion = 2;
        motion(0, 0);
        leg[2].motion = 2;
        motion(2, 0);
        syncLeg();
      }
      ubahGerak = 1;
    }
    leg[0].motion = 2;
    leg[1].motion = 2;
    leg[2].motion = 2;
    leg[3].motion = 2;
    leg[4].motion = 2;
    leg[5].motion = 2;
    motion(0, 0);
    motion(1, 0);
    motion(2, 0);
    motion(3, 0);
    motion(4, 0);
    motion(5, 0);
    syncLeg();
  } else if (arah == 1)  //putar kanan
  {
    if (ubahGerak == 0) {
      while (leg[1].posisi != 1 && leg[3].posisi != 1) {
        leg[1].motion = 1;
        motion(1, 0);
        leg[3].motion = 1;
        motion(3, 0);
        syncLeg();
      }
      ubahGerak = 1;
    }
    leg[0].motion = 1;
    leg[1].motion = 1;
    leg[2].motion = 1;
    leg[3].motion = 1;
    leg[4].motion = 1;
    leg[5].motion = 1;
    motion(0, 0);
    motion(1, 0);
    motion(2, 0);
    motion(3, 0);
    motion(4, 0);
    motion(5, 0);
    syncLeg();
  }
}

// void geser(int arah, int ubahGerak) {
//   if (arah == 0)  //putar kiri
//   {
//     if (ubahGerak == 0) {
//       while (leg[0].posisi != rate4 && leg[2].posisi != rate4) {
//         leg[0].motion = 2;
//         motion(0, 0);
//         leg[2].motion = 2;
//         motion(2, 0);
//         syncLeg();
//       }
//       ubahGerak = 1;
//     }
//     leg[0].motion = 2;
//     leg[1].motion = 2;
//     leg[2].motion = 2;
//     leg[3].motion = 2;
//     motion(0, 0);
//     motion(1, 0);
//     motion(2, 0);
//     motion(3, 0);
//     syncLeg();
//   } else if (arah == 1)  //putar kanan
//   {
//     if (ubahGerak == 0) {
//       while (leg[1].posisi != 1 && leg[3].posisi != 1) {
//         leg[1].motion = 1;
//         motion(1, 0);
//         leg[3].motion = 1;
//         motion(3, 0);
//         syncLeg();
//       }
//       ubahGerak = 1;
//     }
//     leg[0].motion = 1;
//     leg[1].motion = 1;
//     leg[2].motion = 1;
//     leg[3].motion = 1;
//     motion(0, 0);
//     motion(1, 0);
//     motion(2, 0);
//     motion(3, 0);
//     syncLeg();
//   }
// }

// void tuningTangga() {
//   delay(1000);
//   setServo(0, 45, 100, 90);
//   setServo(1, 45, 80, 110);
//   setServo(2, 45, 80, 110);
//   setServo(3, 45, 100, 90);
//   syncWrite();
//   delay(1000);
//   int hitungTangga = 0;
//   while (hitungTangga < 5) {
//     setServo(0, 45, 120, 90);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(0, 15, 120, 90);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(0, 15, 100, 90);
//     syncWrite();
//     delay(delayTanggaTama);

//     setServo(2, 45, 110, 100);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(2, 90, 110, 100);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(2, 90, 60, 100);
//     syncWrite();
//     delay(delayTanggaTama);

//     setServo(0, 45, 100, 90);
//     setServo(1, 45, 80, 110);
//     setServo(2, 45, 80, 110);
//     setServo(3, 45, 100, 90);
//     syncWrite();
//     delay(delayTanggaTama);

//     setServo(3, 45, 120, 90);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(3, 75, 120, 90);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(3, 75, 100, 90);
//     syncWrite();
//     delay(delayTanggaTama);

//     setServo(1, 45, 110, 100);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(1, 0, 110, 100);
//     syncWrite();
//     delay(delayTanggaTama);
//     setServo(1, 0, 60, 110);
//     syncWrite();
//     delay(delayTanggaTama);

//     setServo(0, 45, 100, 90);
//     setServo(1, 45, 80, 110);
//     setServo(2, 45, 80, 110);
//     setServo(3, 45, 100, 90);
//     syncWrite();
//     delay(delayTanggaTama);

//     hitungTangga++;
//   }
// }
*/

void pasangKaki() {
  // servo0_0.writeMicroseconds(gerakServo(45));
  // servo0_1.writeMicroseconds(gerakServo(90));
  // servo0_2.writeMicroseconds(gerakServo(180));

  servo1_0.writeMicroseconds(gerakServo(45));
  servo1_1.writeMicroseconds(gerakServo(90));
  servo1_2.writeMicroseconds(gerakServo(180));

  // servo2_0.writeMicroseconds(gerakServo(45));
  // servo2_1.writeMicroseconds(gerakServo(90));
  // servo2_2.writeMicroseconds(gerakServo(180));

  servo3_0.writeMicroseconds(gerakServo(90 + 45));
  servo3_1.writeMicroseconds(gerakServo(90));
  servo3_2.writeMicroseconds(gerakServo(0));

  servo4_0.writeMicroseconds(gerakServo(90 + 45));
  servo4_1.writeMicroseconds(gerakServo(90));
  servo4_2.writeMicroseconds(gerakServo(0));

  servo5_0.writeMicroseconds(gerakServo(90 + 45));
  servo5_1.writeMicroseconds(gerakServo(90));  //0
  servo5_2.writeMicroseconds(gerakServo(0));
}

void diam() {
  int ata = 45;  //semakin kecil semakin
  int ten = 60;  //semakin kecil semakin
  int baw = 90;  //semakin kecil semakin ketutup
  servo0_0.writeMicroseconds(gerakServo(ata));
  servo0_1.writeMicroseconds(gerakServo(ten));
  servo0_2.writeMicroseconds(gerakServo(baw + 10));
  servo1_0.writeMicroseconds(gerakServo(ata));
  servo1_1.writeMicroseconds(gerakServo(ten));
  servo1_2.writeMicroseconds(gerakServo(baw));
  servo2_0.writeMicroseconds(gerakServo(ata));
  servo2_1.writeMicroseconds(gerakServo(ten));
  servo2_2.writeMicroseconds(gerakServo(baw + 5));

  servo3_0.writeMicroseconds(gerakServo((90 + ata)));
  servo3_1.writeMicroseconds(gerakServo((90 + ten)));
  servo3_2.writeMicroseconds(gerakServo(baw));
  servo4_0.writeMicroseconds(gerakServo((90 + ata) - 5));
  servo4_1.writeMicroseconds(gerakServo((90 + ten)));
  servo4_2.writeMicroseconds(gerakServo(baw));
  servo5_0.writeMicroseconds(gerakServo((90 + ata) + 3));
  servo5_1.writeMicroseconds(gerakServo((90 + ten)));
  servo5_2.writeMicroseconds(gerakServo(baw));
}

void cekData() {
  if (leg[0].posisi == rate2 || leg[1].posisi == rate2 || leg[2].posisi == rate2 || leg[3].posisi == rate2 || leg[4].posisi == rate2 || leg[5].posisi == rate2) {
    if (input != inputBefore) {
      ubahGerak = 0;
      inisialisasiAwal();
      inputBefore = input;
    }
  }
  if (inputBefore == 0) {
    inisialisasiAwal();
    setKaki();
    syncLeg();
  } else if (inputBefore > 0) {
    setKaki();
    directions();
  } else syncLeg();
}

void loop() {
  pasangKaki();
  input = 1;
  cekData();
  // for(int i =0; i<=15; i++){
  //   inverse(x,y,z):
  // }
  // diam()
  // delay(1000);
  // input = 0;
  // cekData();
  // delay(1000);

  // if (ftdi.available() > 0) {
  //   char keyInput = ftdi.read();
  //   switch (keyInput) {
  //     case 'W': //maju
  //       input = 1;
  //       break;
  //     case 'A': //kiri
  //       input = 2;
  //       break;
  //     case 'S': //belakang
  //       input = 3;
  //       break;
  //     case 'D': //kanan
  //       input = 4;
  //       break;
  //     case 'Q': //pivot kiri
  //       input = 5;
  //       break;
  //     case 'E': //pivot kanan
  //       input = 6;
  //       break;
  //     case 'Z': //tambah tinggi body
  //       Zoff = Zoff + 5;
  //       break;
  //     case 'X': //kurangi tinggi body
  //       Zoff = Zoff - 5;
  //       break;
  //     case 'C': //kurangi tinggi body
  //       input = 0;
  //       break;
  //     default:
  //       input = 0;
  //       break;
  //   }
  //   cekData();
  // }
}
