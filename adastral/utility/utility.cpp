#include <utility/utility.hpp>

int Utility::ExecWithParam(const std::vector<std::string>& params)
{
	std::string param_str;
	for (const auto& i : params)
	{
		param_str += i + " ";
	}
	return system(param_str.c_str());
}