#pragma once
#include <boost/asio.hpp>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace MungPlex
{
    //Removes all instances of the characters in charsToRemove from stringToEdit
	static void RemoveChars(std::string& stringToEdit, const char* charsToRemove)
    {
        for (int i = 0; i < strlen(charsToRemove); ++i)
        {
            stringToEdit.erase(remove(stringToEdit.begin(), stringToEdit.end(), charsToRemove[i]), stringToEdit.end());
        }
    }

    static const std::string GetStringFormatting(const int valueType, const bool isSigned, const bool hex)
    {
        if (valueType == FLOAT || valueType == DOUBLE)
            return "%.10f";

        if (hex)
        {
            switch (valueType)
            {
            case INT8:
                return "%02X";
            case INT16:
                return "%04X";
            case INT64:
                return "%016llX";
            default:
                return "%08X";
            }
        }

        switch (valueType)
        {
        case INT8:
            return isSigned ? "%hhi" : "%hhu";
        case INT16:
            return isSigned ? "%hi" : "%hu";
        case INT64:
            return isSigned ? "%lli" : "%llu";
        default:
            return isSigned ? "%li" : "%lu";
        }
    }

    static bool IsValidHexString(std::string input)
    {
        input = input.c_str();
        return std::all_of(input.begin(), input.end(), ::isxdigit);
    }

    static std::string ToHexString(const uint64_t val, const int leftJustify, const bool prepend_0x = false)
    {
        std::stringstream stream;
        stream << std::hex << val;

        std::string result;
        if (leftJustify > static_cast<int>(stream.str().size()))
        {
            result = std::string(leftJustify - stream.str().size(), '0');
        }

        result += stream.str();
        result = "0x" + result;

        if (prepend_0x)
            return result;

        return result.substr(2);
    }

    static std::string RemoveSpacePadding(const std::string& input, const bool removeAll = false)
    {
        std::string output;

        if (removeAll)
        {
			output = input;
            output.erase(remove_if(output.begin(), output.end(), isspace), output.end());
        }
		else //only remove leading/trailing, and duplicate (except first) spaces
        {
            for (int i = 0; i < input.size() - 1; ++i)
            {
                if (input[i] != 0x20 || (input[i] == 0x20 && input[i + 1] != 0x20))
                {
                    output += input[i];
                    continue;
                }
            }

            if (output.back() == 0x20)
                output.resize(output.size() - 1);
        }

        return output;
    }

    static void PopBackTrailingChars(std::string& str, const char ch)
    {
        while (str.back() == ch)
            str.pop_back();
    }

    static void SetQuotationmarks(std::string& str)
    {
        if (str.front() != '"')
            str = '"' + str;

        if (str.back() != '"')
            str.append("\"");
    }

    static void ParsePointerPath(std::vector<int64_t>& pointerPath, const std::string& pointerPathStr)
    {
        pointerPath.clear();
        std::string line;

        if (pointerPathStr.find(',') == std::string::npos)
        {
            line = RemoveSpacePadding(pointerPathStr, true);

            if (!line.empty())
                if (line.front() != '\0')
                    if (IsValidHexString(line))
                        pointerPath.push_back(stoll(line, 0, 16));
        }
        else
        {
            std::stringstream stream;
            stream << pointerPathStr;

            while (std::getline(stream, line, ','))
            {
                line = RemoveSpacePadding(line, true);

                if (line.empty())
                    break;

                if (line.front() == '\0')
                    break;

                if (!IsValidHexString(line))
                    break;

                pointerPath.push_back(stoll(line, 0, 16));
            }
        }
    }

    enum TimeFlags
    {
        YEAR = 1,
        MONTH = 1 << 1,
        DAY = 1 << 2,
        HOUR = 1 << 3,
        MINUTE = 1 << 4,
        SECOND = 1 << 5
    };

    static std::string GetCurrentTimeString(const int flags = HOUR | MINUTE | SECOND)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm* nowTm = std::localtime(&now_time_t);

        if (nowTm == nullptr)
            throw "Error: Could not get local time.";

        std::string timeArgs;

        if (flags & YEAR)
            timeArgs.append("%Y-");

        if (flags & MONTH)
            timeArgs.append("%m-");

        if (flags & DAY)
            timeArgs.append("%d-");

        if (flags & HOUR)
            timeArgs.append("%H-");

        if (flags & MINUTE)
            timeArgs.append("%M-");

        if (flags & SECOND)
            timeArgs.append("%S");

        PopBackTrailingChars(timeArgs, '-');
        std::ostringstream stream;
        stream << std::put_time(nowTm, timeArgs.c_str());
        return stream.str();
    }
}