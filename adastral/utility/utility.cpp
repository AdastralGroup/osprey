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

/*
description:
  recursively delete contents of directory
res:
  0: success
  1: input path doesn't exist
  2: input path isn't a directory
*/
int Utility::DeleteDirectoryContent(const std::filesystem::path& dir)
{
	if (!std::filesystem::exists(dir))
	{
		return 1;
	}
	if (!std::filesystem::is_directory(dir))
	{
		return 2;
	}
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (std::filesystem::is_directory(entry)) {
            DeleteDirectoryContent(entry);
            std::filesystem::remove(entry);
        } else {
            std::filesystem::remove(entry);
        }
    }
	return 0;
}