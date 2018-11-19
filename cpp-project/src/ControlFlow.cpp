#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
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

std::string join(char delimiter, std::vector<std::string> const& inputs)
{
    if (inputs.empty()) return "";
    
    auto result = std::string{};
    int const totalSize = std::accumulate(begin(inputs), end(inputs), 0, [](int currentSize, std::string const& input){ return currentSize + input.size(); });
    result.reserve(totalSize);
    
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
    auto const keywords = filter([](auto const& keyword){return !keyword.empty();}, split<'\n'>(readFromFile("keywords.txt")));
    return keywords;
}

auto contains(std::vector<std::string> const& substrings)
{
    return [substrings](std::string const& string)
           {
               return std::any_of(begin(substrings), end(substrings), [string](std::string const& substring){ return string.find(substring) != std::string::npos; });
           };
}

int main()
{
    auto const controlFlow = join('\n', filter(contains(controlFlowKeywords()), split<'\n'>(readFromFile("yourCode.txt"))));
    std::cout << controlFlow << '\n';
}
