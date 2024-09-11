#include "sys.hpp"

int sys::exec_with_param(const std::vector<std::string> &params)
{
    std::string param_str;
    for (const auto &i : params)
    {
        param_str += i + " ";
    }

    A_printf("%s", param_str.c_str());
    return system(param_str.c_str());
}

/**
description:
  recursively delete contents of directory
res:
  0: success
  1: input path doesn't exist
  2: input path isn't a directory
*/
int sys::delete_directory_content(const std::filesystem::path &dir)
{
    if (!std::filesystem::exists(dir))
    {
        return 1;
    }
    if (!std::filesystem::is_directory(dir))
    {
        return 2;
    }
    for (const auto &entry : std::filesystem::directory_iterator(dir))
    {
        if (std::filesystem::is_directory(entry))
        {
            delete_directory_content(entry);
            std::filesystem::remove(entry);
        }
        else
        {
            std::filesystem::remove(entry);
        }
    }
    return 0;
}

int sys::extract_zip(const std::string &input_file, const std::string &output_file)
{
    A_printf("[sys/Extract] Extracting %s to %s..", input_file.c_str(), output_file.c_str());
    int ret = zip_extract(input_file.c_str(), output_file.c_str(), nullptr, nullptr);
    return ret;
}

tyti::vdf::object sys::parse_vdf_file(std::filesystem::path file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("invalid file!");
    }
    tyti::vdf::object retval = tyti::vdf::read(file);
    return retval;
}

#if _WIN32
inline bool Is64BitWindows()
{
#if _WIN64
    return true;
#else
    USHORT ProcessMachine;
    USHORT NativeMachine;
    BOOL IsWow64 = IsWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine);

    if (IsWow64)
    {
        if (NativeMachine == IMAGE_FILE_MACHINE_AMD64)
        {
            return true;
        }
    }

    return false;
#endif
}
#endif

std::filesystem::path sys::get_steam_path()
{
#if _WIN32
    char valueData[MAX_PATH];
    DWORD valueLen = MAX_PATH;

    // Check if this is 64 bit or 32 bit process
    const char *subKey = Is64BitWindows() ? "SOFTWARE\\WOW6432Node\\Valve\\Steam" : "\\SOFTWARE\\Valve\\Steam";

    // Get steam install dir from registry
    RegGetValueA(HKEY_LOCAL_MACHINE, subKey, "InstallPath", RRF_RT_ANY, nullptr, &valueData, &valueLen);
    if (valueData[0] == 0)
    {
        // Registry key did not exist/had no value
        A_error("Steam not detected!");
        return std::filesystem::path();
    }

    return std::filesystem::path(valueData);
#else
    std::string home = getenv("HOME");
    auto path_normal = std::filesystem::path(home + "/.local/share/Steam/");
    if (std::filesystem::exists(path_normal))
    {
        return std::filesystem::canonical(path_normal);
    }
    auto path_flatpak = std::filesystem::path(home + "/.var/app/com.valvesoftware.Steam/data/Steam/");
    if (std::filesystem::exists(path_flatpak))
    {
        return std::filesystem::canonical(path_flatpak);
    }
    A_error(ErrorLevel::OOPS,"Steam not detected!");
    return std::filesystem::path("");
#endif
}
