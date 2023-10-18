#include <utility/utility.hpp>

int Utility::ExecWithParam(const std::vector<std::string>& params)
{
	std::string param_str;
	for (const auto& i : params)
	{
		param_str += i + " ";
	}

	printf("%s\n", param_str.c_str());
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

/*
desc:
	extract .zstd file
returns:
	0: success
	1: failed to open input file
	2: failed to create zstd context
	3: failed to allocate memory
	4: failed to decompress
*/
int Utility::ExtractZStd(const std::string& szInputFile, const std::string& szOutputFile)
{
	FILE* inputFile = fopen(szInputFile.c_str(), "rb");
	if (!inputFile)
	{
		std::cerr << "[Utility::ExtractZStd] Failed to open input file" << std::endl;
		return 1;
	}

	ZSTD_DCtx* zstdContext = ZSTD_createDCtx();
	if (!zstdContext) {
		std::cerr << "[Utility::ExtractZStd] Failed to create Zstd decompression context." << std::endl;
		fclose(inputFile);
		return 2;
	}

	// Determine the size of the compressed data
	fseek(inputFile, 0, SEEK_END);
	size_t compressedSize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	// Allocate memory for the compressed data
	char* compressedData = (char*)malloc(compressedSize);
	if (!compressedData) {
		std::cerr << "[Utility::ExtractZStd] Failed to allocate memory for compressed data." << std::endl;
		fclose(inputFile);
		ZSTD_freeDCtx(zstdContext);
		return 3;
	}

	// Read the compressed data from the file
	size_t bytesRead = fread(compressedData, compressedSize, 1, inputFile);
	if (bytesRead != compressedSize) {
		std::cerr << "[Utility::ExtractZStd] Failed to read compressed data." << std::endl;
		fclose(inputFile);
		ZSTD_freeDCtx(zstdContext);
		free(compressedData);
		return 4;
	}

	// Close the input file
	fclose(inputFile);

	std::ofstream outfile;
	outfile.open(szOutputFile.c_str());
	outfile << compressedData;
	outfile.close();

	fclose(inputFile);
	ZSTD_freeDCtx(zstdContext);
	free(compressedData);

	return 0;
}

/*
desc:
	extract tar file to directory
returns:
	0: success
	1: failed to open tar file
*/
int Utility::ExtractTar(const std::string& szInputFile, const std::filesystem::path& szOutputDirectory)
{
    struct archive* a = archive_read_new();
    struct archive* ext = archive_write_disk_new();
    struct archive_entry* entry;

    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME);
    archive_write_disk_set_standard_lookup(ext);

    if (archive_read_open_filename(a, szInputFile.c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "[Utility::ExtractTar] Failed to open the tar file: " << archive_error_string(a) << std::endl;
        return 1;
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* entryPath = archive_entry_pathname(entry);
		std::filesystem::path outputPath = szOutputDirectory / entryPath;

        archive_entry_set_pathname(entry, outputPath.string().c_str());
        archive_write_header(ext, entry);

        if (archive_entry_size(entry) > 0) {
            char buff[10240];
            la_ssize_t len;
            while ((len = archive_read_data(a, buff, sizeof(buff))) > 0) {
                archive_write_data(ext, buff, len);
            }
        }

        archive_write_finish_entry(ext);
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);

    return 0;
}