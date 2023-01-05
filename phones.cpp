
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

const uint16_t digits{6u};
const uint32_t maxNumbers{1000000u};

const std::string inputFileName = "phoneNumbers.txt";
const std::string outputFileName = "sortedPhoneNumbers.txt";
const std::string outputFileNameBasic = "sortedPhoneNumbersBasic.txt";

using PhoneNumber = std::uint32_t;
using PhoneNumbers = std::vector<PhoneNumber>;

struct Timer
{
  Timer() :  m_startTime(std::chrono::high_resolution_clock::now())
  {
  }

  Timer(const std::string& name) :  
    m_name(name),
    m_startTime(std::chrono::high_resolution_clock::now())
  {
  }

  ~Timer() {
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - m_startTime;
    std::cout << m_name << ": " << diff.count() << " [s]\n";
  }

private:

  std::string m_name;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};

void generateNumbers(PhoneNumber count)
{
  // not optimal - this really slows down once count approaches MaxNumbers
  // could use some improvements
  Timer timer("GENERATE NUMBERS");
  const PhoneNumber min = 0;
  const PhoneNumber max = maxNumbers - 1;
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
      if (num >= maxNumbers) {
        num = 0;
      }
    }
    gen.insert(num);
    ofs << std::setfill('0') << std::setw(digits) << num << std::endl;
  }
  ofs.close();
}

void writeSorted()
{
  Timer timer("WRITE SORTED");
  
  std::bitset<maxNumbers> bits;

  std::ifstream ifs(inputFileName);
  std::string line;
  while (std::getline(ifs, line)) {
    bits.set(static_cast<PhoneNumber>(std::stoi(line)), true);
  }

  std::ofstream ofs(outputFileName);
  for (PhoneNumber i = 0; i < maxNumbers; ++i) {
    if (bits.test(i)) {
      ofs << std::setfill('0') << std::setw(digits) << i << '\n';
    }
  }
  ofs.close();
}

void writeSortedBasic()
{
  Timer timer("WRITE SORTED BASIC");

  PhoneNumbers phoneNumbers;
  std::ifstream ifs(inputFileName);
  std::string line;
  while (std::getline(ifs, line)) {
    phoneNumbers.emplace_back(static_cast<PhoneNumber>(std::stoi(line)));
  }

  std::sort(std::begin(phoneNumbers), std::end(phoneNumbers));

  std::ofstream ofs(outputFileNameBasic);
  for (const auto phoneNumber : phoneNumbers) {
    ofs << std::setfill('0') << std::setw(digits) << phoneNumber << '\n';
  }
  ofs.close();
}

bool run(std::uint32_t numNumbers)
{
  generateNumbers(numNumbers);

  writeSorted();

  writeSortedBasic();

  std::cout << "Checking results... " << std::endl;
  std::ifstream canon(outputFileNameBasic); 
  std::ifstream candidate(outputFileName); 
  std::string canonLine;
  std::string candidateLine;
  while (std::getline(canon, canonLine)) {
    if (!std::getline(candidate, candidateLine)) {
      return false;
    }
    if (static_cast<PhoneNumber>(std::stoi(canonLine)) != static_cast<PhoneNumber>(std::stoi(candidateLine))) {
      return false;
    }
  }

  return true;
}

int main(int argc, const char *argv[])
{
  std::uint32_t numNumbers = 1000U;
  std::cout << "Parsing arguments" << std::endl;  
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--num" && argc > i + 1) {
      numNumbers = static_cast<std::uint32_t>(std::stoi(argv[i + 1]));
    } 
  }

  std::cout << "Number of digits in a phone number: " << digits << std::endl;
  std::cout << "Maximum number of phone numbers possible: " << maxNumbers << std::endl;
  std::cout << "Number of phone numbers to sort: " << numNumbers << std::endl;

  if constexpr (digits < 4 || digits > 7) {
    std::cout << "ERROR: number of digits must be between [4...7]." << std::endl;
    return EXIT_FAILURE;    
  }

  if (numNumbers > maxNumbers) {
    std::cout << "ERROR: number of phone numbers exceed maximum allowed." << std::endl;
    return EXIT_FAILURE;
  }

  bool ok{false};
  try {
    ok = run(numNumbers);
  }
  catch (const std::exception& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if (!ok) {
    std::cout << "ERROR: phoneNumbers are not as expected" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Results are correct!" << std::endl;
  return EXIT_SUCCESS;
}
