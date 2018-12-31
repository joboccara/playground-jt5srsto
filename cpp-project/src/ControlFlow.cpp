#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

template<char delimiter>
class WordDelimitedBy : public std::string
{};

template<char delimiter>
std::istream& operator>>(std::istream& is, WordDelimitedBy<delimiter>& output)
{
   std::getline(is, output, delimiter);
   return is;
}

template<char delimiter>
std::vector<std::string> split(std::string const& text)
{
    std::istringstream iss(text);
    return std::vector<std::string>((std::istream_iterator<WordDelimitedBy<delimiter>>(iss)),
                                     std::istream_iterator<WordDelimitedBy<delimiter>>());
}

template<typename T, typename Predicate>
std::vector<std::string> filter(Predicate pred, std::vector<T> const& input)
{
    auto results = std::vector<std::string>{};
    std::copy_if(begin(input), end(input), back_inserter(results), pred);
    return results;
}

int totalSize(std::vector<std::string> const& inputs)
{
    return std::accumulate(begin(inputs), end(inputs), 0,
           [](int currentSize, std::string const& input)
           {
               return currentSize + input.size();
           });
}

std::string join(char delimiter, std::vector<std::string> const& inputs)
{
    if (inputs.empty()) return "";
    
    auto result = std::string{};
    result.reserve(totalSize(inputs));
    
    result.insert(end(result), begin(inputs.front()), end(inputs.front()));
    for (auto input = std::next(begin(inputs)); input != end(inputs); ++input)
    {
        result.push_back(delimiter);
        result.insert(end(result), begin(*input), end(*input));
    }
    return result;
}

std::string readFromFile(std::string const& fileName)
{
    auto fileStream = std::ifstream{fileName};
    return std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
}

std::vector<std::string> controlFlowKeywords()
{
    auto const keywords = filter([](auto const& keyword){return !keyword.empty();}, split<'\n'>(readFromFile("Keywords")));
    return keywords;
}

std::regex makeRegex(std::vector<std::string> const& substrings)
{
    if (substrings.empty()) return std::regex(".*");

    auto regexPattern = std::string{'('};
    for (auto const& substring : substrings)
    {
        regexPattern += "\\b" + substring + "\\b|";
    }
    regexPattern.back() = ')';
    return std::regex(regexPattern);
}

bool contains(std::string const& string, std::regex const& regex)
{
    return std::regex_search(string, regex);
}

auto contains(std::regex const& regex)
{
    return [&regex](std::string const& string)
           {
               return contains(string, regex);
           };
}

auto contains(std::regex && regex)
{
    return [regex{std::move(regex)}](std::string const& string)
           {
               return contains(string, regex);
           };
}

int main()
{
    auto const controlFlow = join('\n', filter(contains(makeRegex(controlFlowKeywords())), split<'\n'>(readFromFile("Code"))));
    std::cout << controlFlow << '\n';
}
