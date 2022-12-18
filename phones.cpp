
#include <algorithm>
#include <set>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <random>

const std::string inputFileName = "phoneNumbers.txt";
const std::string outputFileName = "sortedPhoneNumbers.txt";

using PhoneNumber = std::uint32_t;
using PhoneNumbers = std::vector<PhoneNumber>;

struct Timer
{
  Timer() :  m_startTime(std::chrono::high_resolution_clock::now())
  {
  }

  ~Timer() {
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - m_startTime;
    std::cout << diff.count() << " [s]\n";
  }

private:

  std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};

template <PhoneNumber MaxNumbers>
void generateNumbers(PhoneNumber count)
{
  // not optimal - this really slows down once count approaches MaxNumbers
  // could use some improvements
  Timer timer;
  const auto digits = static_cast<std::uint16_t>(std::log10(MaxNumbers));
  const PhoneNumber min = 0;
  const PhoneNumber max = MaxNumbers;
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<PhoneNumber> uni(min, max);
  std::set<PhoneNumber> gen;
  srand(static_cast<unsigned int>(time(nullptr)));
  std::ofstream ofs(inputFileName);
  for (std::size_t i = 0; i < count; ++i) {
    PhoneNumber num = uni(rng);
    while (gen.count(num)) {
      ++num;
    }
    gen.insert(num);
    ofs << std::setfill('0') << std::setw(digits) << num << std::endl;
  }
  ofs.close();
}

PhoneNumbers getPhoneNumbers()
{
  Timer timer;
  PhoneNumbers numbers;

  std::ifstream ifs(inputFileName);

  std::string line;
  while (std::getline(ifs, line)) {
    numbers.emplace_back(static_cast<PhoneNumber>(std::stoi(line)));
  }

  return numbers;
}

template <PhoneNumber MaxNumbers>
void sortPhoneNumbers(PhoneNumbers& phoneNumbers)
{ 
  Timer timer;

  std::vector<bool> bits(MaxNumbers, false);

  std::for_each(std::begin(phoneNumbers), std::end(phoneNumbers), [&](const auto& phoneNumber){bits[phoneNumber] = true;});

  PhoneNumber j = 0;
  for (PhoneNumber i = 0; i < MaxNumbers && j < phoneNumbers.size(); ++i) {
    if (bits.at(i)) {
      phoneNumbers[j++] = i;
    }
  }
}

void sortBasic(PhoneNumbers& phoneNumbers)
{
  Timer timer;
  std::sort(std::begin(phoneNumbers), std::end(phoneNumbers));
}

template <PhoneNumber MaxNumbers>
void putPhoneNumbers(const PhoneNumbers& phoneNumbers)
{
  Timer timer;
  const auto digits = static_cast<std::uint16_t>(std::log10(MaxNumbers));  
  std::ofstream ofs(outputFileName);
  for (const auto& phoneNumber : phoneNumbers) {
    ofs << std::setfill('0') << std::setw(digits) << phoneNumber << '\n';
  }
  ofs.close();
}

template <std::uint32_t MaxNumbers>
bool run(std::uint32_t numNumbers)
{
  std::cout << "Generating " << numNumbers << " numbers... ";
  generateNumbers<MaxNumbers>(numNumbers);

  std::cout << "Reading numbers... ";
  auto phoneNumbers = getPhoneNumbers();

  // make a copy for later use
  auto phoneNumbersCopy = phoneNumbers;
  
  std::cout << "Sorting numbers using the clever method... ";
  sortPhoneNumbers<MaxNumbers>(phoneNumbers);

  std::cout << "Writing sorted numbers... ";
  putPhoneNumbers<MaxNumbers>(phoneNumbers);

  std::cout << "Sorting numbers using std::sort... ";
  sortBasic(phoneNumbersCopy);

  if (!std::equal(phoneNumbers.begin(), phoneNumbers.end(), phoneNumbersCopy.begin())) {
    std::cout << "ERROR: phoneNumbers are not as expected" << std::endl;
    return false;
  }  

  return true;
}

int main(int argc, const char *argv[])
{
  std::uint32_t numNumbers = 1000U;
  std::uint16_t digits = 6;
  std::cout << "Parsing arguments" << std::endl;  
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--num" && argc > i + 1) {
      numNumbers = static_cast<std::uint32_t>(std::stoi(argv[i + 1]));
    }
    if (std::string(argv[i]) == "--digits" && argc > i + 1) {
      digits = static_cast<std::uint16_t>(std::stoi(argv[i + 1]));
    }    
  }

  std::uint32_t maxNumbers = static_cast<std::uint32_t>(std::pow(10, digits));

  std::cout << "Number of digits in a phone number: " << digits << std::endl;
  std::cout << "Maximum number of phone numbers possible: " << maxNumbers << std::endl;
  std::cout << "Number of phone numbers to sort: " << numNumbers << std::endl;

  if (digits < 4 || digits > 7) {
    std::cout << "ERROR: number of digits must be between [4...7]." << std::endl;
    return EXIT_FAILURE;    
  }

  if (numNumbers > maxNumbers) {
    std::cout << "ERROR: number of phone numbers exceed maximum allowed." << std::endl;
    return EXIT_FAILURE;
  }

  bool ok{false};
  if (digits == 4) {
    ok = run<10000>(numNumbers);
  }
  else if (digits == 5) {
    ok = run<100000>(numNumbers);
  }
  else if (digits == 6) {
    ok = run<1000000>(numNumbers);
  }
  else if (digits == 7) {
    ok = run<10000000>(numNumbers);
  }

  if (!ok) {
    std::cout << "ERROR: phoneNumbers are not as expected" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Results are correct!" << std::endl;
  return EXIT_SUCCESS;
}
