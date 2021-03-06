#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "system/Filesystem.hpp"
#include "system/Logger.hpp"

namespace jse {

	namespace fs = std::filesystem;

	FileSystem::FileSystem()
	{
		mWorkingPath = fs::current_path();
	}

	void FileSystem::SetWorkingDir( const String& aWorkingDir )
	{
		String tmp = aWorkingDir;
#ifdef WIN32
		for (int i = 0; i < tmp.length(); ++i)
		{
			if (tmp[i] == '/') tmp[i] = '\\';
		}
#endif
		mWorkingPath = tmp;
	}

	const String FileSystem::GetWorkingDir() const
	{
		return mWorkingPath.string();
	}

	const String FileSystem::Resolve(const String& aPath) const
	{
		String tmp = aPath;
#ifdef WIN32
		for (int i = 0; i < tmp.length(); ++i)
		{
			if (tmp[i] == '/') tmp[i] = '\\';
		}
#endif
		const fs::path p = mWorkingPath / tmp;
		return p.string();
	}

	bool FileSystem::ReadTextFile(const String& asFile, String &aOutput)
	{

		std::ifstream ifs(asFile.c_str(), std::ios::in);
		if (ifs.is_open())
		{
			std::stringstream sstr;
			sstr << ifs.rdbuf();
			aOutput = sstr.str();
			ifs.close();
		}
		else
		{
			Error("Cannot load file %s", asFile.c_str());

			return false;
		}

		return true;
	}

	bool FileSystem::ReadTextFileBase(const String& asFile, String& aOutput)
	{
		return ReadTextFile(Resolve(asFile), aOutput);
	}

	ByteVector FileSystem::ReadBinaryFile(const String& aFileName)
	{
		ByteVector result;

		std::ifstream input(aFileName, std::ios::binary);

		if (input.good())
		{
			// copies all data into buffer
			ByteVector buffer(std::istreambuf_iterator<char>(input), {});

			result = buffer;

			input.close();
		}

		return result;
	}
}
