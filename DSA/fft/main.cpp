#include <algorithm>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
using std::cout;
using std::sort;
using std::vector;
#define _USE_MATH_DEFINES

const double cDva = 2.0;

std::pair<int, int> Read(std::vector<std::complex<double>>& a,
                         std::vector<std::complex<double>>& b) {
  int a_size;
  int b_size;
  {
    std::cin >> a_size;
    ++a_size;
    a.resize(a_size);
    for (int i = 0; i < a_size; ++i) {
      int x;
      std::cin >> x;
      a[a_size - 1 - i] = x;
    }
  }
  {
    std::cin >> b_size;
    ++b_size;
    b.resize(b_size);
    for (int i = 0; i < b_size; ++i) {
      int x;
      std::cin >> x;
      b[b_size - 1 - i] = x;
    }
  }
  return {a_size, b_size};
}

int ReverseNum(int x, int bits_count) {
  int y = 0;
  for (int i = 0; i < bits_count; ++i) {
    y |= (((1 << i) & x) > 0 ? (1 << ((bits_count - 1) - i)) : 0);
  }
  return y;
}

void Unite(std::vector<std::complex<double>>& ret,
           const std::vector<std::complex<double>>& complex_value,
           int group_size, int cur_pos, int step, int deg) {
  for (int i = 0; i < group_size; ++i) {
    int pos = i + cur_pos;
    std::complex<double> x = ret[pos];
    std::complex<double> y = ret[pos + group_size];

    int idx = i * (1 << (deg - step - 1));
    idx %= (int)complex_value.size();

    std::complex<double> w = complex_value[idx];
    ret[pos] = x + w * y;
    ret[pos + group_size] = x - w * y;
  }
}

std::vector<std::complex<double>> Fft(
    std::vector<std::complex<double>>& arr,
    std::vector<std::complex<double>>& complex_value, int deg) {
  int n = 1 << deg;
  std::vector<std::complex<double>> ret(n, 0);
  for (int i = 0; i < (int)arr.size(); ++i) {
    ret[i] = arr[i];
  }

  for (int i = 0; i < n; ++i) {
    int rev_i = ReverseNum(i, deg);
    if (i < rev_i) {
      std::swap(ret[i], ret[rev_i]);
    }
  }

  for (int step = 0; step < deg; ++step) {
    int group_size = 1 << step;
    int groups_count = n / (2 * group_size);
    for (int group = 0; group < groups_count; ++group) {
      int cur_pos = group * 2 * group_size;
      Unite(ret, complex_value, group_size, cur_pos, step, deg);
    }
  }
  return ret;
}

void Solve() {
  using namespace std::complex_literals;
  std::vector<std::complex<double>> a;
  std::vector<std::complex<double>> b;
  int a_size;
  int b_size;
  std::tie(a_size, b_size) = Read(a, b);

  int total_size = a_size + b_size;
  int deg = 0;
  while ((1 << deg) < total_size) {
    deg++;
  }
  int n = 1 << deg;

  a.resize(n, 0);
  b.resize(n, 0);

  std::vector<std::complex<double>> complex_value(n);
  for (int i = 0; i < n; ++i) {
    double angle = cDva * M_PI * i / n;
    complex_value[i] = std::complex<double>(std::cos(angle), std::sin(angle));
  }

  std::vector<std::complex<double>> a_fft = Fft(a, complex_value, deg);
  std::vector<std::complex<double>> b_fft = Fft(b, complex_value, deg);

  std::vector<std::complex<double>> ab_fft(n);
  for (int i = 0; i < n; ++i) {
    ab_fft[i] = a_fft[i] * b_fft[i];
  }

  std::vector<std::complex<double>> conjugate_complex(n);
  for (int i = 0; i < n; ++i) {
    conjugate_complex[i] = std::conj(complex_value[i]);
  }

  std::vector<std::complex<double>> restore_fft =
      Fft(ab_fft, conjugate_complex, deg);

  for (int i = 0; i < n; ++i) {
    restore_fft[i] /= n;
  }

  int last_pos = n - 1;
  while (static_cast<int>(std::round(restore_fft[last_pos].real())) == 0) {
    last_pos--;
  }
  std::cout << last_pos << ' ';
  for (int i = last_pos; i >= 0; --i) {
    std::cout << static_cast<int>(std::round(restore_fft[i].real())) << ' ';
  }
}

int main() {
  Solve();
  return 0;
}
