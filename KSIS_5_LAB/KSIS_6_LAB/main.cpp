#include <iostream>
#include <bitset>

using namespace std;
unsigned long ip, mask;

// Функция для конвертации строки IP-адреса в значение типа unsigned long
unsigned long CharToLong(char* ip_) {
  unsigned long out = 0; 
  char* buff; 
  buff = new char[3]; 

  for (int i = 0, j = 0, k = 0; ip_[i] != '\0'; i++, j++) {
    if (ip_[i] != '.') 
      buff[j] = ip_[i];
    if (ip_[i] == '.' || ip_[i + 1] == '\0') { 
      out <<= 8; 
      if (atoi(buff) > 255) 
        return NULL; 
      out += (unsigned long)atoi(buff); 
      k++; 
      j = -1; 
      delete[] buff; 
      buff = new char[3]; 
    }
  }
  return out; 
}

// Функция для проверки корректности IP-адреса
bool CheckAddress(char* ip_) {
  int points = 0, numbers = 0; 
  char* buff = new char[3]; 

  for (int i = 0; ip_[i] != '\0'; i++) {
    if (ip_[i] <= '9' && ip_[i] >= '0') { 
      if (numbers > 3) return false; 
      buff[numbers++] = ip_[i];
    }
    else if (ip_[i] == '.') { 
      if (atoi(buff) > 255) 
        return false;
      if (numbers == 0) 
        return false;
      numbers = 0; 
      points++; 
      delete[] buff; 
      buff = new char[3]; 
    }
    else return false; 
  }
  if (points != 3)
    return false;
  if (numbers == 0 || numbers > 3)
    return false;
  return true; 
}

// Функция для проверки корректности маски подсети
bool checkMask(char* mask_) {
  int points = 0, numbers = 0; 
  char* buff = new char[3]; 
  for (int i = 0; mask_[i] != '\0'; i++) {
    if (mask_[i] <= '9' && mask_[i] >= '0') { 
      if (numbers > 3) return false; 
      buff[numbers++] = mask_[i]; 
    }
    else if (mask_[i] == '.' || mask_[i] == '\0') { 
      if (atoi(buff) > 255) 
        return false;
      if (numbers == 0) 
        return false;
      numbers = 0; 
      points++; 
      delete[] buff; 
      buff = new char[3]; 
    }
    else return false; 
  }

  unsigned long mask = CharToLong(mask_); 
  bitset<32> bytemask = (bitset<32>(mask)); 

  if (points != 3)
    return false;
  if (numbers == 0 || numbers > 3)
    return false;
  for (size_t i = 31; i >= 1; i--) {
    if (bytemask[i] == 0 && bytemask[i - 1] == 1) 
      return false;
  }
  return true; 
}

int main() {
  setlocale(0, ""); 
  unsigned long ip, mask; 
  char* ip_ = new char[16], * mask_ = new char[16];
  bool flag = true; 


  do {
    if (!flag) cout << "Неправильный IP\n" << endl; 
    cout << "Введите IP: \n";
    cin >> ip_; 
  } while (!(flag = CheckAddress(ip_))); 

  do {
    if (!flag) cout << "Неправильная маска\n" << endl; 
    cout << "Введите маску: \n";
    cin >> mask_; 
  } while (!(flag = checkMask(mask_))); 

  mask = CharToLong(mask_);
  ip = CharToLong(ip_);

  unsigned long subnet = ip & mask;
  unsigned char bytes[4]; 

  bytes[0] = (subnet & 0xFF);
  bytes[1] = (subnet >> 8) & 0xFF;
  bytes[2] = (subnet >> 16) & 0xFF;
  bytes[3] = (subnet >> 24) & 0xFF;

  cout << "ID подсети: ";
  for (int i = 3; i > 0; i--) {
    cout << (int)bytes[i] << '.';
  }
  cout << (int)bytes[0] << '\n';

  unsigned long IDhost = ip & ~mask;
  bytes[0] = (IDhost & 0xFF);
  bytes[1] = (IDhost >> 8) & 0xFF;
  bytes[2] = (IDhost >> 16) & 0xFF;
  bytes[3] = (IDhost >> 24) & 0xFF;

  cout << "ID хоста: ";
  for (int i = 3; i > 0; i--) {
    cout << (int)bytes[i] << '.';
  }
  cout << (int)bytes[0] << '\n';

  unsigned long broadcast = (ip & mask | ~mask);

  bytes[0] = (broadcast & 0xFF);
  bytes[1] = (broadcast >> 8) & 0xFF;
  bytes[2] = (broadcast >> 16) & 0xFF;
  bytes[3] = (broadcast >> 24) & 0xFF;

  cout << "Broadcast адрес: ";
  for (int i = 3; i > 0; i--) {
    cout << (int)bytes[i] << '.';
  }
  cout << (int)bytes[0] << '\n';

  delete[] ip_;
  delete[] mask_;

  return 0;
}