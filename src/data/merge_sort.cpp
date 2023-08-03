#include "data/merge_sort.h"
#include <algorithm>

std::vector<std::string> Merge(std::vector<std::string>& left, std::vector<std::string>& right);
bool CompareStrings(const std::string& left, const std::string& right);

std::vector<std::string> MergeSort(const std::vector<std::string>& strings)
{
    // Base case
    const size_t size = strings.size();
    if (size <= 1) return strings;

    // Split in half
    std::vector<std::string> left(strings.begin(), strings.begin() + size / 2);
    std::vector<std::string> right(strings.begin() + size / 2, strings.end());

    // Perform merges recursively
    auto sorted_left = MergeSort(left);
    auto sorted_right = MergeSort(right);
    return Merge(sorted_left, sorted_right);
}

std::vector<std::string> Merge(std::vector<std::string>& left, std::vector<std::string>& right)
{
    const size_t total_size = left.size() + right.size();
    std::vector<std::string> results;

    size_t i = 0;
    size_t j = 0;

    for (size_t k = 0; k < total_size; ++k)
    {
        // Both left and right have options left
        if (left.size() > i && right.size() > j)
        {
            // Compare the two and add the one that comes first
            if (CompareStrings(left[i], right[j]))
                results.emplace_back(left[i++]);
            else
                results.emplace_back(right[j++]);
        }

        // No more left pairs
        else if (left.size() <= i)
            results.emplace_back(right[j++]);

        // No more right pairs
        else if (right.size() <= j)
            results.emplace_back(left[i++]);
    }

    return results;
}

bool CompareStrings(const std::string& left, const std::string& right)
{
    return std::min(left, right) == left;
}
